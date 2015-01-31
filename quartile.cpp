
#include <iostream>
#include <string>
#include <algorithm>
#include <iomanip>
#include <ios>
#include <vector>

int quart () {
    using namespace std;

    cout << "Enter a list of numbers: ";

    vector<double> quantile;
    double x;
    //invariant: homework contains all the homework grades so far
    while (cin >> x)
        quantile.push_back(x);

    //check that the student entered some homework grades
    //typedef vector<double>::size_type vec_sz;
    int size = quantile.size();

    if (size == 0) {
        cout << endl << "You must enter your numbers . "
                        "Please try again." << endl;
        return 1;
    }

    auto const Q1 = quantile.size() / 4;
    auto const Q2 = quantile.size() / 2;
    auto const Q3 = Q1 + Q2;

    std::nth_element(quantile.begin(),          quantile.begin() + Q1, quantile.end());
    std::nth_element(quantile.begin() + Q1 + 1, quantile.begin() + Q2, quantile.end());
    std::nth_element(quantile.begin() + Q2 + 1, quantile.begin() + Q3, quantile.end());

    int mid = size/2;
    double median;
    median = size % 2 == 0 ? (quantile[mid] + quantile[mid-1])/2 : quantile[mid];

    vector<double> first;
    vector<double> third;

    for (int i = 0; i!=mid; ++i)
    {
        first.push_back(quantile[i]);
    }

    for (int i = mid; i!= size; ++i)
    {
        third.push_back(quantile[i]);
    }
        double fst;
        double trd;

        int side_length = 0;

        if (size % 2 == 0)
        {
            side_length = size/2;
        }
        else {
            side_length = (size-1)/2;
        }

        fst = (size/2) % 2 == 0 ? (first[side_length/2]/2 + first[(side_length-1)/2])/2 : first[side_length/2];
        trd = (size/2) % 2 == 0 ? (third[side_length/2]/2 + third[(side_length-1)/2])/2 : third[side_length/2];

    streamsize prec = cout.precision();
    cout << "The quartiles are " <<  setprecision(3) << " 1st "
        << fst << " 2nd " << median << " 3rd " << trd << setprecision(prec) << endl;

    return 0;

}
