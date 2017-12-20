#ifndef LOGIC_H
#define LOGIC_H

#include "Primitives.h"
#include <vector>

using namespace std;

class Logic
{
    private:

    const double epsilon = 1e-7;
    const double near_edge = 1e1;

    enum {
        convex = 0,
        concave = 1
    };

    vector<bool> concavity;
    vector<Point> polygon;
    int no_vertex;

    bool trigonometric_order(const Point &A, const Point &B, const Point &C);
    bool point_inside(const Point & query, const Triangle & T);
    bool check_for_ear(const vector<Point> & polygon_remote, const int & position, const vector<bool> & concavity_remote);
    bool equal(const double &a, const double &b);
    bool equalPoint(const Point &a, const Point &b);
    Point ask_for_a_ray(const Point &act, const Point & fst, const Point & lst);
    void rotate_polygon(vector<Point> & polygon_remote, double angle);
    double get_angle(const Point &A, const Point &B, const Point &C);
    int ask_for_second_position(const Point &Y, const Point &Pi, const Point &Pk, vector<Point> & polygon_remote, int my_k, const Point &pred);
    void ask_for_a_polygon(vector<Point> & polygon_remote, int a, int b, vector<bool> & concavity_remote);
    Point FindAnEar(vector<Point> polygon_remote, const int & position, vector<bool> concavity_remote);
    void dump(vector<Point> polygon);
    void sort_points_by_convexity(vector<Point> & polygon, vector<bool> & concavity);
    Point middle(const Point &a, const Point &b, const Point &c);
    double distance(const Point &a, const Point &b);
    bool is_on_edge(const Point &query, const Point &A, const Point &B);

    public:

    Logic(const vector<Point> & polygon_received);
    vector<Triangle> get_triangulation();
    int is_point_inside(const Point & p);
    void replace_polygon(const vector<Point> & polygon_received);
    pair<Point, Point> check_for_edges(const Point &query);
};

#endif