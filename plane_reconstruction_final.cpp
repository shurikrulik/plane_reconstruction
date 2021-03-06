#include <iostream>
#include <fstream>
#include <vector>
#include <math.h> 
#include <iomanip> 
#include <stdexcept>

#define EPS 1e-10

using std::ostream;  using std::istream;  using std::endl;
using std::domain_error;

#ifndef __MATRIX_H__
#define __MATRIX_H__

class Matrix {
    public:
        Matrix(int, int);
        Matrix();
        ~Matrix();
        Matrix(const Matrix&);
        Matrix& operator=(const Matrix&);

        inline double& operator()(int x, int y) { return p[x][y]; }

        Matrix& operator+=(const Matrix&);
        Matrix& operator-=(const Matrix&);
        Matrix& operator*=(const Matrix&);
        Matrix& operator*=(double);
        Matrix& operator/=(double);
        Matrix  operator^(int);
        
        friend std::ostream& operator<<(std::ostream&, const Matrix&);
        friend std::istream& operator>>(std::istream&, Matrix&);

        void swapRows(int, int);
        Matrix transpose();

        static Matrix createIdentity(int);
        static Matrix solve(Matrix, Matrix);
        static Matrix bandSolve(Matrix, Matrix, int);

        static double dotProduct(Matrix, Matrix);

        static Matrix augment(Matrix, Matrix);
        Matrix gaussianEliminate();
        Matrix rowReduceFromGaussian();
        void readSolutionsFromRREF(std::ostream& os);
        Matrix inverse();

    private:
        int rows_, cols_;
        double **p;

        void allocSpace();
        Matrix expHelper(const Matrix&, int);
};

Matrix operator+(const Matrix&, const Matrix&);
Matrix operator-(const Matrix&, const Matrix&);
Matrix operator*(const Matrix&, const Matrix&);
Matrix operator*(const Matrix&, double);
Matrix operator*(double, const Matrix&);
Matrix operator/(const Matrix&, double);

#endif

Matrix::Matrix(int rows, int cols) : rows_(rows), cols_(cols)
{
    allocSpace();
    for (int i = 0; i < rows_; ++i) {
        for (int j = 0; j < cols_; ++j) {
            p[i][j] = 0;
        }
    }
}

Matrix::Matrix() : rows_(1), cols_(1)
{
    allocSpace();
    p[0][0] = 0;
}

Matrix::~Matrix()
{
    for (int i = 0; i < rows_; ++i) {
        delete[] p[i];
    }
    delete[] p;
}

Matrix::Matrix(const Matrix& m) : rows_(m.rows_), cols_(m.cols_)
{
    allocSpace();
    for (int i = 0; i < rows_; ++i) {
        for (int j = 0; j < cols_; ++j) {
            p[i][j] = m.p[i][j];
        }
    }
}

Matrix& Matrix::operator=(const Matrix& m)
{
    if (this == &m) {
        return *this;
    }

    if (rows_ != m.rows_ || cols_ != m.cols_) {
        for (int i = 0; i < rows_; ++i) {
            delete[] p[i];
        }
        delete[] p;

        rows_ = m.rows_;
        cols_ = m.cols_;
        allocSpace();
    }

    for (int i = 0; i < rows_; ++i) {
        for (int j = 0; j < cols_; ++j) {
            p[i][j] = m.p[i][j];
        }
    }
    return *this;
}

Matrix& Matrix::operator+=(const Matrix& m)
{
    for (int i = 0; i < rows_; ++i) {
        for (int j = 0; j < cols_; ++j) {
            p[i][j] += m.p[i][j];
        }
    }
    return *this;
}

Matrix& Matrix::operator-=(const Matrix& m)
{
    for (int i = 0; i < rows_; ++i) {
        for (int j = 0; j < cols_; ++j) {
            p[i][j] -= m.p[i][j];
        }
    }
    return *this;
}

Matrix& Matrix::operator*=(const Matrix& m)
{
    Matrix temp(rows_, m.cols_);
    for (int i = 0; i < temp.rows_; ++i) {
        for (int j = 0; j < temp.cols_; ++j) {
            for (int k = 0; k < cols_; ++k) {
                temp.p[i][j] += (p[i][k] * m.p[k][j]);
            }
        }
    }
    return (*this = temp);
}

Matrix& Matrix::operator*=(double num)
{
    for (int i = 0; i < rows_; ++i) {
        for (int j = 0; j < cols_; ++j) {
            p[i][j] *= num;
        }
    }
    return *this;
}

Matrix& Matrix::operator/=(double num)
{
    for (int i = 0; i < rows_; ++i) {
        for (int j = 0; j < cols_; ++j) {
            p[i][j] /= num;
        }
    }
    return *this;
}

Matrix Matrix::operator^(int num)
{
    Matrix temp(*this);
    return expHelper(temp, num);
}

void Matrix::swapRows(int r1, int r2)
{
    double *temp = p[r1];
    p[r1] = p[r2];
    p[r2] = temp;
}

Matrix Matrix::transpose()
{
    Matrix ret(cols_, rows_);
    for (int i = 0; i < rows_; ++i) {
        for (int j = 0; j < cols_; ++j) {
            ret.p[j][i] = p[i][j];
        }
    }
    return ret;
}


Matrix Matrix::createIdentity(int size)
{
    Matrix temp(size, size);
    for (int i = 0; i < temp.rows_; ++i) {
        for (int j = 0; j < temp.cols_; ++j) {
            if (i == j) {
                temp.p[i][j] = 1;
            } else {
                temp.p[i][j] = 0;
            }
        }
    }
    return temp;
}

Matrix Matrix::solve(Matrix A, Matrix b)
{
    for (int i = 0; i < A.rows_; ++i) {
        if (A.p[i][i] == 0) {
            throw domain_error("Error: the coefficient matrix has 0 as a pivot. Please fix the input and try again.");
        }
        for (int j = i + 1; j < A.rows_; ++j) {
            for (int k = i + 1; k < A.cols_; ++k) {
                A.p[j][k] -= A.p[i][k] * (A.p[j][i] / A.p[i][i]);
                if (A.p[j][k] < EPS && A.p[j][k] > -1*EPS)
                    A.p[j][k] = 0;
            }
            b.p[j][0] -= b.p[i][0] * (A.p[j][i] / A.p[i][i]);
            if (A.p[j][0] < EPS && A.p[j][0] > -1*EPS)
                A.p[j][0] = 0;
            A.p[j][i] = 0;
        }
    }

    Matrix x(b.rows_, 1);
    x.p[x.rows_ - 1][0] = b.p[x.rows_ - 1][0] / A.p[x.rows_ - 1][x.rows_ - 1];
    if (x.p[x.rows_ - 1][0] < EPS && x.p[x.rows_ - 1][0] > -1*EPS)
        x.p[x.rows_ - 1][0] = 0;
    for (int i = x.rows_ - 2; i >= 0; --i) {
        int sum = 0;
        for (int j = i + 1; j < x.rows_; ++j) {
            sum += A.p[i][j] * x.p[j][0];
        }
        x.p[i][0] = (b.p[i][0] - sum) / A.p[i][i];
        if (x.p[i][0] < EPS && x.p[i][0] > -1*EPS)
            x.p[i][0] = 0;
    }

    return x;
}

Matrix Matrix::bandSolve(Matrix A, Matrix b, int k)
{
    int bandsBelow = (k - 1) / 2;
    for (int i = 0; i < A.rows_; ++i) {
        if (A.p[i][i] == 0) {
            throw domain_error("Error: the coefficient matrix has 0 as a pivot. Please fix the input and try again.");
        }
        for (int j = i + 1; j < A.rows_ && j <= i + bandsBelow; ++j) {
            int k = i + 1;
            while (k < A.cols_ && A.p[j][k]) {
                A.p[j][k] -= A.p[i][k] * (A.p[j][i] / A.p[i][i]);
                k++;
            }
            b.p[j][0] -= b.p[i][0] * (A.p[j][i] / A.p[i][i]);
            A.p[j][i] = 0;
        }
    }

    Matrix x(b.rows_, 1);
    x.p[x.rows_ - 1][0] = b.p[x.rows_ - 1][0] / A.p[x.rows_ - 1][x.rows_ - 1];
    for (int i = x.rows_ - 2; i >= 0; --i) {
        int sum = 0;
        for (int j = i + 1; j < x.rows_; ++j) {
            sum += A.p[i][j] * x.p[j][0];
        }
        x.p[i][0] = (b.p[i][0] - sum) / A.p[i][i];
    }

    return x;
}

double Matrix::dotProduct(Matrix a, Matrix b)
{
    double sum = 0;
    for (int i = 0; i < a.rows_; ++i) {
        sum += (a(i, 0) * b(i, 0));
    }
    return sum;
}

Matrix Matrix::augment(Matrix A, Matrix B)
{
    Matrix AB(A.rows_, A.cols_ + B.cols_);
    for (int i = 0; i < AB.rows_; ++i) {
        for (int j = 0; j < AB.cols_; ++j) {
            if (j < A.cols_)
                AB(i, j) = A(i, j);
            else
                AB(i, j) = B(i, j - B.cols_);
        }
    }
    return AB;
}

Matrix Matrix::gaussianEliminate()
{
    Matrix Ab(*this);
    int rows = Ab.rows_;
    int cols = Ab.cols_;
    int Acols = cols - 1;

    int i = 0; // row iterator
    int j = 0; // column iterator

    // iterate through the rows
    while (i < rows)
    {
        // find a pivot for the row
        bool pivot_found = false;
        while (j < Acols && !pivot_found)
        {
            if (Ab(i, j) != 0) { // pivot not equal to 0
                pivot_found = true;
            } else { // check for a possible swap
                int max_row = i;
                double max_val = 0;
                for (int k = i + 1; k < rows; ++k)
                {
                    double cur_abs = Ab(k, j) >= 0 ? Ab(k, j) : -1 * Ab(k, j);
                    if (cur_abs > max_val)
                    {
                        max_row = k;
                        max_val = cur_abs;
                    }
                }
                if (max_row != i) {
                    Ab.swapRows(max_row, i);
                    pivot_found = true;
                } else {
                    j++;
                }
            }
        }

        // perform elimination as normal if pivot was found
        if (pivot_found)
        {
            for (int t = i + 1; t < rows; ++t) {
                for (int s = j + 1; s < cols; ++s) {
                    Ab(t, s) = Ab(t, s) - Ab(i, s) * (Ab(t, j) / Ab(i, j));
                    if (Ab(t, s) < EPS && Ab(t, s) > -1*EPS)
                        Ab(t, s) = 0;
                }
                Ab(t, j) = 0;
            }
        }

        i++;
        j++;
    }

    return Ab;
}

Matrix Matrix::rowReduceFromGaussian()
{
    Matrix R(*this);
    int rows = R.rows_;
    int cols = R.cols_;

    int i = rows - 1; 
    int j = cols - 2; 

    // iterate through every row
    while (i >= 0)
    {
        // find the pivot column
        int k = j - 1;
        while (k >= 0) {
            if (R(i, k) != 0)
                j = k;
            k--;
        }

        // zero out elements above pivots if pivot not 0
        if (R(i, j) != 0) {
       
            for (int t = i - 1; t >= 0; --t) {
                for (int s = 0; s < cols; ++s) {
                    if (s != j) {
                        R(t, s) = R(t, s) - R(i, s) * (R(t, j) / R(i, j));
                        if (R(t, s) < EPS && R(t, s) > -1*EPS)
                            R(t, s) = 0;
                    }
                }
                R(t, j) = 0;
            }

            // divide row by pivot
            for (int k = j + 1; k < cols; ++k) {
                R(i, k) = R(i, k) / R(i, j);
                if (R(i, k) < EPS && R(i, k) > -1*EPS)
                    R(i, k) = 0;
            }
            R(i, j) = 1;

        }

        i--;
        j--;
    }

    return R;
}

void Matrix::readSolutionsFromRREF(ostream& os)
{
    Matrix R(*this);

    // print number of solutions
    bool hasSolutions = true;
    bool doneSearching = false;
    int i = 0;
    while (!doneSearching && i < rows_)
    {
        bool allZeros = true;
        for (int j = 0; j < cols_ - 1; ++j) {
            if (R(i, j) != 0)
                allZeros = false;
        }
        if (allZeros && R(i, cols_ - 1) != 0) {
            hasSolutions = false;
            os << "NO SOLUTIONS" << endl << endl;
            doneSearching = true;
        } else if (allZeros && R(i, cols_ - 1) == 0) {
            os << "INFINITE SOLUTIONS" << endl << endl;
            doneSearching = true;
        } else if (rows_ < cols_ - 1) {
            os << "INFINITE SOLUTIONS" << endl << endl;
            doneSearching = true;
        }
        i++;
    }
    if (!doneSearching)
        os << "UNIQUE SOLUTION" << endl << endl;

    // get solutions if they exist
    if (hasSolutions)
    {
        Matrix particular(cols_ - 1, 1);
        Matrix special(cols_ - 1, 1);

        for (int i = 0; i < rows_; ++i) {
            bool pivotFound = false;
            bool specialCreated = false;
            for (int j = 0; j < cols_ - 1; ++j) {
                if (R(i, j) != 0) {
                    // if pivot variable, add b to particular
                    if (!pivotFound) {
                        pivotFound = true;
                        particular(j, 0) = R(i, cols_ - 1);
                    } else { // otherwise, add to special solution
                        if (!specialCreated) {
                            special = Matrix(cols_ - 1, 1);
                            specialCreated = true;
                        }
                        special(j, 0) = -1 * R(i, j);
                    }
                }
            }
            os << "Special solution:" << endl << special << endl;
        }
        os << "Particular solution:" << endl << particular << endl;
    }
}

Matrix Matrix::inverse()
{
    Matrix I = Matrix::createIdentity(rows_);
    Matrix AI = Matrix::augment(*this, I);
    Matrix U = AI.gaussianEliminate();
    Matrix IAInverse = U.rowReduceFromGaussian();
    Matrix AInverse(rows_, cols_);
    for (int i = 0; i < AInverse.rows_; ++i) {
        for (int j = 0; j < AInverse.cols_; ++j) {
            AInverse(i, j) = IAInverse(i, j + cols_);
        }
    }
    return AInverse;
}


void Matrix::allocSpace()
{
    p = new double*[rows_];
    for (int i = 0; i < rows_; ++i) {
        p[i] = new double[cols_];
    }
}

Matrix Matrix::expHelper(const Matrix& m, int num)
{
    if (num == 0) { 
        return createIdentity(m.rows_);
    } else if (num == 1) {
        return m;
    } else if (num % 2 == 0) {  // num is even
        return expHelper(m * m, num/2);
    } else {                    // num is odd
        return m * expHelper(m * m, (num-1)/2);
    }
}


Matrix operator+(const Matrix& m1, const Matrix& m2)
{
    Matrix temp(m1);
    return (temp += m2);
}

Matrix operator-(const Matrix& m1, const Matrix& m2)
{
    Matrix temp(m1);
    return (temp -= m2);
}

Matrix operator*(const Matrix& m1, const Matrix& m2)
{
    Matrix temp(m1);
    return (temp *= m2);
}

Matrix operator*(const Matrix& m, double num)
{
    Matrix temp(m);
    return (temp *= num);
}

Matrix operator*(double num, const Matrix& m)
{
    return (m * num);
}

Matrix operator/(const Matrix& m, double num)
{
    Matrix temp(m);
    return (temp /= num);
}

ostream& operator<<(ostream& os, const Matrix& m)
{
    for (int i = 0; i < m.rows_; ++i) {
        os << m.p[i][0];
        for (int j = 1; j < m.cols_; ++j) {
            os << " " << m.p[i][j];
        }
        os << endl;
    }
    return os;
}

istream& operator>>(istream& is, Matrix& m)
{
    for (int i = 0; i < m.rows_; ++i) {
        for (int j = 0; j < m.cols_; ++j) {
            is >> m.p[i][j];
        }
    }
    return is;
}
using namespace std;

struct point3d
{
    double x;
    double y;
    double z;
    double distance;
};

bool belonging_of_point_to_plane(double a, double b, double c, double d, double x, double y, double z, double p){
    return fabs(a*x+b*y+c*z+d) <= p;
}

vector<double> plane_equation_coefficients_by_3points(double x1,double y1,double z1,double x2,double y2,double z2,double x3,double y3,double z3){
    double a1 = x2 - x1;
    double b1 = y2 - y1;
    double c1 = z2 - z1;
    double a2 = x3 - x1;
    double b2 = y3 - y1;
    double c2 = z3 - z1;
    double a = b1 * c2 - b2 * c1;
    double b = a2 * c1 - a1 * c2;
    double c = a1 * b2 - b1 * a2;
    double d = (-a * x1 - b * y1 - c * z1);
    vector<double> coefficients{a, b, c, d};
    return coefficients;
}

void quick_sort_vector_by_distance(vector<point3d> &points_cloud, vector<point3d>::size_type left, vector<point3d>::size_type right)
{ 
  int left_edge = left; 
  int right_edge = right;
  double pvt;

  pvt = points_cloud[left].distance;
  while (left < right){
    while ((points_cloud[right].distance >= pvt) && (left < right))
      right--; 
    if (left != right){
        points_cloud[left] = points_cloud[right]; 
        left++; 
    }
    while ((points_cloud[left].distance <= pvt) && (left < right))
        left++; 
    if (left != right){
        points_cloud[right] = points_cloud[left]; 
        right--; 
    }
  }

  points_cloud[left].distance = pvt; 
  pvt = left;
  left = left_edge;
  right = right_edge;
  if (left < pvt) 
    quick_sort_vector_by_distance(points_cloud, left, pvt - 1);
  if (right > pvt)
    quick_sort_vector_by_distance(points_cloud, pvt + 1, right);
}

double length_of_perpendicular_to_plane(double a, double b, double c, double d, double x1, double y1, double z1) 
{ 
    double k = (-a * x1 - b * y1 - c * z1 - d) / (double)(a * a + b * b + c * c); 
    double x2 = a * k + x1; 
    double y2 = b * k + y1; 
    double z2 = c * k + z1; 

    double ax = x2 - x1;
    double ay = y2 - y1;
    double az = z2 - z1;
    double vector_length = sqrt(pow(ax,2) + pow(ay,2) + pow(az,2));
  
    return vector_length;
} 

void read_file(string path_to_file, double &p, int &number_of_points, vector<point3d> &points_cloud){
    point3d point;
    ifstream read(path_to_file);
    if(!read.eof()){
        read >> p;
        read >> number_of_points;
        while(!read.eof()){
            read >> point.x;
            read >> point.y;
            read >> point.z;
            points_cloud.push_back(point);
        }
    }
}

int main(){
    int number_of_points;
    double a, b, c, d, x, y, z, p;
    vector<point3d> points_cloud;
    string path_to_file = "input.txt";
    ofstream write("output.txt");
    read_file(path_to_file, p, number_of_points, points_cloud);
    double perpendicular_length = 0;
    vector<double> current_coefficients{0,0,0,0};
    vector<double> most_fitted_coefficients{0,0,0,0};
    double current_fitment_score = 0;
    double most_fitment_score = 0;
   
        for(vector<point3d>::size_type i = 0; i <= points_cloud.size()-3; i++) {
        current_coefficients = plane_equation_coefficients_by_3points(   points_cloud[i].x,points_cloud[i].y,points_cloud[i].z,
                                                                    points_cloud[i+1].x,points_cloud[i+1].y,points_cloud[i+1].z,
                                                                    points_cloud[i+2].x,points_cloud[i+2].y,points_cloud[i+2].z);
            if(fabs(current_coefficients[0]) + fabs(current_coefficients[1]) + fabs(current_coefficients[2]) != 0 ){
                current_fitment_score = 0;
                for(vector<point3d>::size_type i = 0; i <= points_cloud.size()-3; i+=3) {
                    if(fabs(points_cloud[i].x*current_coefficients[0]+points_cloud[i].y*current_coefficients[1]+points_cloud[i].z*current_coefficients[2]+current_coefficients[3])<=p){
                        current_fitment_score++;
                    }
                }
                current_fitment_score/=number_of_points;
                if(current_fitment_score > most_fitment_score) {
                    most_fitted_coefficients = current_coefficients;
                    most_fitment_score = current_fitment_score;
                }
            }
        }
        for(vector<point3d>::size_type i = 0; i < points_cloud.size(); i++) {
            points_cloud[i].distance = length_of_perpendicular_to_plane(most_fitted_coefficients[0], most_fitted_coefficients[1],
                                                                        most_fitted_coefficients[2],most_fitted_coefficients[3],
                                                                        points_cloud[i].x,points_cloud[i].y,points_cloud[i].z);
        }
        quick_sort_vector_by_distance(points_cloud, 0, points_cloud.size()-1);

        Matrix A(points_cloud.size()/2, 3), B(points_cloud.size()/2, 1), fitness(3, 1);

        for(vector<point3d>::size_type i = 0; i < points_cloud.size()/2; i++) {
            A(i, 0) = points_cloud[i].x;
            A(i, 1) = points_cloud[i].y;
            A(i, 2) = 1;
            B(i, 0) = points_cloud[i].z;
        }
        fitness = (A.transpose() * A).inverse() * A.transpose() * B;
        a = fitness(0, 0) * -1;
        b = fitness(1, 0) * -1;
        c = 1;
        d = fitness(2, 0) * -1;
    write.precision(6);
    write<<fixed<<a<<" "<<b<<" "<<c<<" "<<d;

    return 0;
}
