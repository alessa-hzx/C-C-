#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



struct date_tag {
  unsigned year;
  unsigned month;
};
typedef struct date_tag date_t;

struct ror_arr_tag {
    double * ror_sp;
    double * ror_bond;
};
typedef struct ror_arr_tag ror_arr_t;

// struct to hold array of dates and cumulative investments                           
// (hint: can be used for any investment strategy)                                    
struct ret_portfolio_tag {
  date_t * dates;
  double * cum_invest;
  size_t n;
};
typedef struct ret_portfolio_tag ret_portfolio_t;



// This function reads a line that has n strings seperated by "," 
// and points to the strings in an array of char (malloc)
char ** Parseline(char * line, int n){
    char ** arr = malloc(sizeof(* arr) * n);
    if (arr == NULL){
        perror("malloc fails.\n");
        exit(EXIT_FAILURE);
    }
    char * p = line;
    for (int i = 0; i < n; i++){
        arr[i] = p;
        p = strchr(arr[i], ',');
        if (strchr(arr[i], ',') != NULL){
            * p = '\0';
            p++;
        }
    }
    return arr;
}


// this function reads date value year.month into date_t
date_t readdate(char * date_str){
    date_t date;
    char * year_str = date_str;
    char * p = strchr(year_str, '.');
    if (strchr(year_str, '.') != NULL){
        * p = '\0';
    }else{
        fprintf(stderr, "Wrong date format.\n");
        exit(EXIT_FAILURE);
    }
    date.year = atoi(year_str);
    char * month_str = p + 1;
    if (strchr(month_str, '.') != NULL){
        fprintf(stderr, "Wrong date format.\n");
        exit(EXIT_FAILURE);
    }
    date.month = atoi(month_str);
    if ((date.month > 12) || (date.month == 0)){
        fprintf(stderr, "Enter a valid month.\n");
        exit(EXIT_FAILURE);
    }
    return date;
}

// this fucntion determines whether two dates are the same
int same(date_t a, date_t b){
    if ((a.month == b.month) & (a.year == b.year)){
        return 1;
    }
    return 0;
}

// this function determines whether current date (now) is between start and end dates
// but does not includ the start date
int between(date_t now, date_t start, date_t end){
    int after_start = 0;
    int before_end = 0;
    if (now.year == start.year){
        if(now.month > start.month){
            after_start = 1;
        }
    } else if (now.year > start.year){
        after_start = 1;
    }
    if (now.year == end.year){
        if (now.month <= end.month){
            before_end = 1;
        }
    } else if (now.year < end.year){
        before_end = 1;
    }
    return after_start * before_end;
}



// this function reads the sp500 data and compute the rate of return for all dates
double * returnsp(FILE * f_sp, date_t startdate, date_t enddate){
    double * ret_sp = NULL;
    int n = 0; // number of returns counted
    double p1, p2, div, r;
    p1 = 0;
    int match = 0; // whether the start and end date match the dates available in the data
    char * line = NULL;
    size_t sz = 0;
    while(getline(&line, &sz, f_sp)>=0){
        if (isdigit(*line)){
            char ** arr = Parseline(line, 3);
            date_t now = readdate(arr[0]);
            if (same(now, startdate) || (same(now, enddate))){
                match++;
            }
            p2 = atof(arr[1]);
            if ((p1 != 0) &  between(now, startdate, enddate)){
                n++;
                ret_sp = realloc(ret_sp, sizeof(* ret_sp)* n);
                div = atof(arr[2]);
                r = (p2/p1 - 1) + (div/12)/p2;
                ret_sp[n-1] = r;
            } 
            p1 = p2;
            free(arr);
        }
        free(line);
        line = NULL;
        
    }
    free(line);
    
    if (match < 2){
        fprintf(stderr, "No data available for input dates.\n");
        exit(EXIT_FAILURE);
    }
    return ret_sp;
}

// this fucntion computes the series return rate of us bond between start and end date
double * returnbond(FILE * f_bond, date_t start, date_t end){
    double * ret_bond = NULL;
    int n = 0; // number of days
    char * line = NULL;
    size_t sz = 0;
    while (getline(&line, &sz, f_bond) >= 0){
        if (isdigit(*line)){
            char ** arr = Parseline(line, 2);
            date_t now = readdate(arr[0]);
            double p = atof(arr[1]);
            if (between(now, start, end)){
                n++;
                ret_bond = realloc(ret_bond, sizeof(* ret_bond) * n);
                ret_bond[n-1] = p / 12/100;
            }
            free(arr);
        }
        free(line);
        line = NULL;
    }
    free(line);
    return ret_bond;
}

// this function computes the cum investment every day given rate of returns
// type: 1 for sp500, 2 for bond, 3 for lifecycle
ret_portfolio_t * bal(double * ror_sp, double * ror_bond, date_t start, date_t end, int invest_type){
    double bal = 0;
    double contrib = 100;
    double w_sp = 1; // weight for sp500
    double * ror;
    if (invest_type == 1){
        ror = ror_sp;
    } else if (invest_type == 2){
        ror = ror_bond;
    }
    ret_portfolio_t * cum = malloc(sizeof(* cum));
    cum ->dates = NULL;
    cum -> cum_invest = NULL;
    size_t n = 0;
    for(int y = start.year; y <= end.year; y++){
        for (int m = 1; m <= 12; m++){
            date_t now;
            now.month = m;
            now.year = y;
            if ((n > 0) & (m == 1)){
                contrib *= 1.025; // monthly contribution increases in each Jan.
                w_sp -= 0.02; 
            }
            if (between(now, start, end)){
                n++;
                cum->dates = realloc(cum->dates, sizeof(*(cum->dates)) * n);
                cum->cum_invest = realloc(cum->cum_invest, sizeof(*(cum->cum_invest))* n);
                cum->dates[n-1] = now;
                if (invest_type != 3){
                    bal = bal * (1 + ror[n-1]) + contrib;
                } else{
                    bal = bal * (1 + ror_sp[n-1]) * w_sp + bal * (1 + ror_bond[n-1]) * (1-w_sp)
                    + contrib;
                }
                cum->cum_invest[n-1] = bal;
            }
        }
    }
    cum->n = n;
    return cum;
}


// this function frees the memory allocated to the portfolio information
void free_cum(ret_portfolio_t * cum){
    free(cum->cum_invest);
    free(cum->dates);
    free(cum);
}

// this function prints the output on file f
void printout(FILE * f, ret_portfolio_t * cum1, ret_portfolio_t * cum2, ret_portfolio_t * cum3){
    date_t * d = cum1->dates;
    double * bal1 = cum1->cum_invest;
    double * bal2 = cum2->cum_invest;
    double * bal3 = cum3->cum_invest;

    for (size_t i = 0; i < cum1->n; i++){
        fprintf(f, "%u.%02u,%.2f,%.2f,%.2f\n", d->year, d->month, *bal1, *bal2, *bal3);
        d++;
        bal1++;
        bal2++;
        bal3++;
    }
}

int main(int argc, char ** argv){
    // arguments
    if (argc != 5){
        fprintf(stderr, "Wrong number of arguments!\n");
        return EXIT_FAILURE;
    }
    // read sp500 data
    FILE * f_sp = fopen(argv[1], "r");
    if (f_sp == NULL){
        perror("Could not open sp500 file!\n");
        return EXIT_FAILURE;
    }
    // read bond data
    FILE * f_bond = fopen(argv[2], "r");
    if (f_bond == NULL){
        perror("Could not open bond file!\n");
        return EXIT_FAILURE;
    }
    date_t start = readdate(argv[3]);
    date_t end = readdate(argv[4]);
    /* ############################################################ */
    if (same(start, end)){
        fprintf(stderr, "Choose different start and end date.\n");
        return EXIT_FAILURE;
    }
    /* ############################################################# */
    ror_arr_t ror;
    ror.ror_sp  = returnsp(f_sp, start, end); // compute returns for sp500
    ror.ror_bond = returnbond(f_bond, start, end); // returns for bond
    ret_portfolio_t * cum_sp = bal(ror.ror_sp, ror.ror_bond, start, end, 1);
    ret_portfolio_t * cum_bond = bal(ror.ror_sp, ror.ror_bond, start, end, 2);
    ret_portfolio_t * cum_life = bal(ror.ror_sp, ror.ror_bond, start, end, 3);
    // print output
    FILE * fout = fopen("portfolio.txt", "w");
    printout(fout, cum_sp, cum_bond, cum_life);
    // free memo
    free_cum(cum_sp);
    free_cum(cum_bond);
    free_cum(cum_life);
    // close files
    if (fclose(fout) != 0){
        perror("Fail to close the output file.\n");
        return EXIT_FAILURE;
    }
    if (fclose(f_sp) != 0){
        perror("Fail to close the sp500 file.\n");
        return EXIT_FAILURE;
    }
    if (fclose(f_bond) != 0){
        perror("Failed to close the bond file.\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
