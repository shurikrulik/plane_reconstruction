#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <bits/stdc++.h> 
#include <iomanip> 

using namespace std;

bool belonging_of_point_to_plane(double a, double b, double c, double d, double x, double y, double z, double p){
    return fabs(a*x+b*y+c*z+d) <= p;
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

void read_file(string path_to_file, double &p, int &number_of_points, vector<double> &points_cloud){
    double point=0;
    ifstream read(path_to_file);
    if(!read.eof()){
        read >> p;
        read >> number_of_points;
        while(!read.eof()){
            read >> point;
            points_cloud.push_back(point);
        }
    }
}

int main(){
    int number_of_points;
    double a, b, c, d, x, y, z, p;
    vector<double> points_cloud;
    //string path_to_file = "input.txt";
    string path_to_file = "sdc_point_cloud.txt";
    ofstream write("output.txt");
    read_file(path_to_file, p, number_of_points, points_cloud);
    //cout<<p<<'\n'<<number_of_points<<'\n';
    cout << std::fixed; 
    cout << std::setprecision(6);
    double perpendicular_length = 0;
    vector<double> current_coefficients{0,0,0,0};
    vector<double> most_fitted_coefficients{0,0,0,0};
    double current_fitment_score = 0;
    double most_fitment_score = 0;
    if(number_of_points < 600){
        for(vector<double>::size_type i = 0; i <= points_cloud.size()-9; i++) {
        current_coefficients = plane_equation_coefficients_by_3points(   points_cloud[i],points_cloud[i+1],points_cloud[i+2],
                                                                    points_cloud[i+3],points_cloud[i+4],points_cloud[i+5],
                                                                    points_cloud[i+6],points_cloud[i+7],points_cloud[i+8]);
            //cout<<current_coefficients[0]<<" "<<current_coefficients[1]<<" "<<current_coefficients[2]<<" "<<current_coefficients[3];
            if(fabs(current_coefficients[0]) + fabs(current_coefficients[1]) + fabs(current_coefficients[2]) + fabs(current_coefficients[3]) != 0 ){
                current_fitment_score = 0;
                for(vector<double>::size_type i = 0; i <= points_cloud.size()-3; i+=3) {
                    if(fabs(points_cloud[i]*current_coefficients[0]+points_cloud[i+1]*current_coefficients[1]+points_cloud[i+2]*current_coefficients[2]+current_coefficients[3])<=p){
                        current_fitment_score++;
                    }
                }
                current_fitment_score/=number_of_points;
                //cout<<" "<<current_fitment_score*100<<"%"<<endl;
                if(current_fitment_score > most_fitment_score) {
                    most_fitted_coefficients = current_coefficients;
                    most_fitment_score = current_fitment_score;
                }
            }
        }
    }
    else{

             for(vector<double>::size_type i = 0; i <= points_cloud.size()-18; i+=9) {
        current_coefficients = plane_equation_coefficients_by_3points(   points_cloud[i],points_cloud[i+1],points_cloud[i+2],
                                                                    points_cloud[i+3],points_cloud[i+4],points_cloud[i+5],
                                                                    points_cloud[i+6],points_cloud[i+7],points_cloud[i+8]);
            //cout<<current_coefficients[0]<<" "<<current_coefficients[1]<<" "<<current_coefficients[2]<<" "<<current_coefficients[3];
            if(fabs(current_coefficients[0]) + fabs(current_coefficients[1]) + fabs(current_coefficients[2]) + fabs(current_coefficients[3]) != 0 ){
                current_fitment_score = 0;
                for(vector<double>::size_type i = 0; i <= points_cloud.size()-3; i+=3) {
                    if(fabs(points_cloud[i]*current_coefficients[0]+points_cloud[i+1]*current_coefficients[1]+points_cloud[i+2]*current_coefficients[2]+current_coefficients[3])<=p){
                        current_fitment_score++;
                    }
                }
                current_fitment_score/=number_of_points;
                //cout<<" "<<current_fitment_score*100<<"%"<<endl;
                if(current_fitment_score > most_fitment_score) {
                    most_fitted_coefficients = current_coefficients;
                    most_fitment_score = current_fitment_score;
                }
            }
        }

    }
    cout<<"Most fitment score: "<<most_fitment_score*100<<"%"<<" Most fitted coefficients: "<<most_fitted_coefficients[0]<<" "<<most_fitted_coefficients[1]<<" "<<most_fitted_coefficients[2]<<" "<<most_fitted_coefficients[3]<<endl;
   // int format_output=0;
   /* for(double n : points_cloud) {
        cout<<n<<" ";
        format_output++;
       if(format_output % 3 == 0){
            cout<<'\n';
       }
    }*/
   /* cout << std::fixed; 
    cout << std::setprecision(6);
    perpendicular_length = length_of_perpendicular_to_plane(0.000000, 0.000000, 1.000000,-0.995037, 20,0,3);
    cout<<"perpendicular length = "<<perpendicular_length<<endl;*/
    /*if(belonging_of_point_to_plane(-0.099504, 0.000000, 0.995037,0.000000,10,10,0.1,0.01))
        cout<<"Belongs !"<<endl;
    else
        cout<<"Doesn't belongs ("<<endl;*/
/*15,-10,0.15,10,10,0.1,20,-10,0.2*/
   /* vector<double> coefficients = plane_equation_coefficients_by_3points(20, 0,	3,10, -10, 2, 10, 10, 2);
    for(double n : coefficients) {
        cout<<n<<" ";
    }
    cout<<endl;*/
    /*if(belonging_of_point_to_plane(coefficients[0], coefficients[1], coefficients[2],coefficients[3],20, 0, 0,0.01))
        cout<<"Belongs !"<<endl;
    else
        cout<<"Doesn't belongs ("<<endl;*/
    return 0;
}