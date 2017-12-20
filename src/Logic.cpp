
#include "Logic.h"
#include <vector>
#include <cmath>
#include <cassert>

using namespace std;

bool Logic :: trigonometric_order(const Point &A, const Point &B, const Point &C)
{
    if( (A.x - B.x) * (A.y - C.y) - (A.y - B.y) * (A.x - C.x) <= 0)
        return true;
    else
        return false;
}

bool Logic :: point_inside(const Point & query, const Triangle & T)
{
    bool t1, t2, t3;

    t1 = trigonometric_order(query, T.A, T.B);
    t2 = trigonometric_order(query, T.B, T.C);
    t3 = trigonometric_order(query, T.C, T.A);

    if(t1 == t2 && t1 == t3)
        return true;
    return false;
}

bool Logic :: check_for_ear(const vector<Point> & polygon_remote, const int & position, const vector<bool> & concavity_remote)
{
    if(concavity_remote[position] == convex)
    {
        int pred = (position == 0 ? polygon_remote.size() - 1 : position - 1);
        int next = (position == polygon_remote.size() - 1) ? 0 : position + 1;

        for(int i = 0; i < polygon_remote.size(); i++)
        {
            if(i != position && i != pred && i != next)
            {
                if(concavity_remote[i] == concave && point_inside(polygon_remote[i], Triangle{polygon_remote[position], polygon_remote[pred], polygon_remote[next]}) == true)
                    return false;
            }
        }
        return true;
    }
    return false;
}

bool Logic :: equal(const double &a, const double &b)
{
    double dif = a - b;

    if(dif < 0)
        dif = - dif;

    return dif < epsilon;
}

bool Logic :: equalPoint(const Point &a, const Point &b)
{
    return (equal(a.x, b.x) && equal(a.y, b.y));
}

Point Logic :: ask_for_a_ray(const Point &act, const Point & fst, const Point & lst)
{
    if(equal(fst.x, lst.x))
    {
        if(equal(fst.x, act.x))
        {
            if(fst.y < lst.y)
                return fst;
            else
                return lst;
        }
        else
        {
            return act;
        }
    }

    double a, b, c;

    a = fst.y - lst.y;
    b = lst.x - fst.x;

    c = - a * lst.x - b * lst.y; 

    double proper_y = (- c - act.x * a) / b;

    if(fst.x <= act.x && act.x <= lst.x)
    {
        return {act.x, proper_y};
    }

    if(fst.x >= act.x && act.x >= lst.x)
    {
        return {act.x, proper_y};
    }

    return act;
}

void Logic :: rotate_polygon(vector<Point> & polygon_remote, double angle)
{
    double my_cos = cos(angle), my_sin = sin(angle);

    for(int i = 0; i < polygon_remote.size(); i++)
    {
        double fx, fy;
        fx = my_cos * polygon_remote[i].x - my_sin * polygon_remote[i].y;
        fy = my_sin * polygon_remote[i].x + my_cos * polygon_remote[i].y;

        polygon_remote[i].x = fx;
        polygon_remote[i].y = fy;
    }
}

double Logic :: get_angle(const Point &A, const Point &B, const Point &C)
{
    double angle = atan2(A.y - C.y, A.x - C.x);
    if(angle < 0)
        return -angle;
    return angle;
}


int Logic :: ask_for_second_position(const Point &Y, const Point &Pi, const Point &Pk, vector<Point> & polygon_remote, int my_k, const Point &pred)
{
    vector<int> R;

    for(int i = 0; i < polygon_remote.size(); i++)
    {
        Point th = polygon_remote[i];
        if(equalPoint(th, Pi) || equalPoint(th, Y) || equalPoint(th, Pk))
        {
            continue;
        }

        if(point_inside(th, Triangle{Pi, Y, Pk}) == true)
        {
            R.push_back(i);
        }
    }

    if(R.size() == 0)
    {
        
        if(equalPoint(Pk, pred) == 0)
            return my_k;
        return -1;
    }

    int best = 0;
    double best_angle = 4 * acos(0);

    for(int i = 0; i < R.size(); i++)
    {
        double act_angle = get_angle(Y, Pi, polygon_remote[ R[i] ] );

        if(act_angle < best_angle)
        {
            best_angle = act_angle;
            best = i;
        }
    }

    if(equalPoint(pred, polygon_remote[best]) == 0)
    {
        return best;
    }
    return -1;
}

void Logic :: ask_for_a_polygon(vector<Point> & polygon_remote, int a, int b, vector<bool> & concavity_remote)
{
    if(b < a)
    {
        swap(a, b);
    }

    vector<Point> new_polygon;
    vector<bool> new_concavity;

    for(int i = a; i <= b; i++)
    {
        new_polygon.push_back(polygon_remote[i]);
        new_concavity.push_back(concavity_remote[i]);
    }

    polygon_remote.clear();
    polygon_remote = new_polygon;

    concavity_remote.clear();
    concavity_remote = new_concavity;
}

Point Logic :: FindAnEar(vector<Point> polygon_remote, const int & position, vector<bool> concavity_remote)
{
    if( polygon_remote.size() < 7)
    {
        for(int i = 0; i < polygon_remote.size(); i++)
        {
            if(check_for_ear(polygon_remote, position, concavity_remote) == true)
            {
                return polygon_remote[position];
            }
        }
    }

    if(check_for_ear(polygon_remote, position, concavity_remote) == true)
    {
        return polygon_remote[position];
    }

    int pred, next;


    if(position == 0)
        pred = polygon_remote.size() - 1;
    else
        pred = position - 1;

    if(position == polygon_remote.size() - 1)
        next = 0;
    else
        next = position + 1;

    Point asoc = {(polygon_remote[pred].x + polygon_remote[next].x) / 2, (polygon_remote[pred].y + polygon_remote[next].y) / 2};

    double angle = atan2(asoc.y - polygon_remote[position].y, asoc.x - polygon_remote[position].x);

    double pi = acos(0);
    pi *= 2;


    if(concavity_remote[position] == concave)
    {
        angle += pi;
    }

    angle += pi / 2; 
    angle = 2 * pi - angle;

    rotate_polygon(polygon_remote, angle);

    Point act = polygon_remote[position];

    Point proper_y =  {0, -2000000000};
    int proper_k = -1;


    for(int i = 0; i < polygon_remote.size(); i++)
    {
        Point fst = polygon_remote[i];
        Point lst;

        if(i == polygon_remote.size() - 1)
            lst = polygon_remote[0];
        else
            lst = polygon_remote[i + 1];

        Point needed_y = ask_for_a_ray(polygon_remote[position], fst, lst);

        if(equalPoint(needed_y, polygon_remote[position]) || equalPoint(needed_y, polygon_remote[pred]) || equalPoint(needed_y, polygon_remote[next]))
            continue;


        if(needed_y.y >= act.y)
            continue;

        if(needed_y.y > proper_y.y)
        {
            proper_y = needed_y;
            proper_k = i;
        }
    }

    //assert(proper_k >= 0);

    assert (equalPoint(proper_y, polygon_remote[position]) == 0 );

    for(int i = 0; i < polygon_remote.size(); i++)
    {
        if(equalPoint(proper_y, polygon_remote[i]))
        {
            ask_for_a_polygon(polygon_remote, position, i, concavity_remote);


            rotate_polygon(polygon_remote, 2 * pi - angle);

            if(polygon_remote.size() == 3)
            {
                return polygon_remote[1];
            }

            return FindAnEar(polygon_remote, polygon_remote.size() / 2, concavity_remote);    
        }
    }


    int second_position = 0;

    if(proper_k == polygon_remote.size() - 1)
        second_position = ask_for_second_position(proper_y, act, polygon_remote[0], polygon_remote, 0, polygon_remote[pred]);
    else
        second_position = ask_for_second_position(proper_y, act, polygon_remote[proper_k + 1], polygon_remote, proper_k + 1, polygon_remote[pred]);

    if(second_position >= 0 )
    {
        ask_for_a_polygon(polygon_remote, position, second_position, concavity_remote);
        rotate_polygon(polygon_remote, 2 * pi - angle);

        if(polygon_remote.size() == 3)
        {
            return polygon_remote[1];
        }

        return FindAnEar(polygon_remote, polygon_remote.size() / 2, concavity_remote);  
    }

    second_position = ask_for_second_position(proper_y, act, polygon_remote[proper_k], polygon_remote, proper_k, polygon_remote[next]);

    if(second_position >= 0)
    {
        ask_for_a_polygon(polygon_remote, position, second_position, concavity_remote);
        rotate_polygon(polygon_remote, 2 * pi - angle);

        if(polygon_remote.size() == 3)
        {
            return polygon_remote[1];
        }

        return FindAnEar(polygon_remote, polygon_remote.size() / 2, concavity_remote);  
    } 

    throw "belea";

    assert(false);
    
}

void Logic :: dump(vector<Point> polygon)
{
    for(int i = 0; i < polygon.size(); i++)
    {
        printf("%.5lf %.5lf\n", polygon[i].x, polygon[i].y);
    }

    printf("\n\n");
}

void Logic :: sort_points_by_convexity(vector<Point> & polygon, vector<bool> & concavity)
{
    int left_most_index;

    if(polygon.front().x < polygon.back().x)
        left_most_index = 0;
    else
        left_most_index = polygon.size() - 1;

    vector<int> trigo_list;


    for(int i = 1; i < polygon.size() - 1; i++)
    {
        if(polygon[i].x < polygon[left_most_index].x)
            left_most_index = i;

        if(trigonometric_order(polygon[i - 1], polygon[i], polygon[i + 1]) == true)
        {
            trigo_list.push_back(i);
        }
    }

    if(trigonometric_order(polygon[polygon.size() - 2], polygon[polygon.size() - 1], polygon[0]) == true)
    {
        trigo_list.push_back(polygon.size() - 1);
    }
    if(trigonometric_order(polygon[polygon.size() - 1], polygon[0], polygon[1]) == true)
    {
        trigo_list.push_back(0);
    }

    bool left_in_list = false;

    for(int i = 0; i < trigo_list.size(); i++)
    {
        if(trigo_list[i] == left_most_index)
            left_in_list =  true;
    }

    concavity.clear();
    concavity.resize(polygon.size());

    if(left_in_list)
    {
        for(int i = 0; i < polygon.size(); i++)
        {
            concavity[i] = true;
        }

        for(int i = 0; i < trigo_list.size(); i++)
        {
            concavity[trigo_list[i]] = false;
        }
    }
    else
    {
        for(int i = 0; i < trigo_list.size(); i++)
        {
            concavity[trigo_list[i]] = true;
        }
    }
}

Point Logic :: middle(const Point &a, const Point &b, const Point &c)
{
    if( equal(a.x, b.x) == false)
    {
        if((a.x<b.x && b.x<c.x) || (a.x>b.x && b.x>c.x))
            return b;
        if((a.x<c.x && c.x<b.x) || (a.x>c.x && c.x>b.x))
            return c;
        if((b.x<a.x && a.x<c.x) || (b.x>a.x && a.x>c.x) )
            return a;
    }
    else
    {
        if((a.y<b.y && b.y<c.y) || (a.y>b.y && b.y>c.y))
            return b;
        if((a.y<c.y && c.y<b.y) || (a.y>c.y && c.y>b.y))
            return c;
        if((b.y<a.y && a.y<c.y) || (b.y>a.y && a.y>c.y))
            return a;
    }
    return b;
}

double Logic :: distance(const Point &a, const Point &b)
{
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

bool Logic :: is_on_edge(const Point &query, const Point &A, const Point &B)
{
    double a, b, c;

    a = A.y - B.y;
    b = B.x - A.x;

    c = - A.x * a - A.y * b;

    double proper_y;

    if(equal(b, 0) == 0 )
    {
        proper_y = (- a * query.x - c) / b;
    }
    else
    {
        proper_y = A.y;
    }

    double dif = proper_y - query.y;
    if(dif < 0)
        dif = -dif;

    Point found = {query.x, proper_y};

    if(equalPoint( found, middle(found, A, B)) == 1 && dif < near_edge)
        return true;
    if(distance(query, A) < near_edge * near_edge)
        return true;
    
    if(distance(query, B) < near_edge * near_edge)
        return true;

    return false;
}

vector<Triangle> Logic :: get_triangulation() {

    sort_points_by_convexity(polygon, concavity);

    vector<Triangle> solution;

    if(polygon.size() == 3)
    {
        solution.push_back({polygon[0], polygon[1], polygon[2]});
        return solution;
    }

    if(polygon.size() < 3)
    {
        return solution;
    }

    vector<Point> polygon_copy = polygon;
    vector<bool> concavity_copy = concavity;


    for(int step = 0; polygon_copy.size() > 3; step++)
    {
        sort_points_by_convexity(polygon_copy, concavity_copy);//OK

        Point ear_mon = FindAnEar(polygon_copy, polygon_copy.size() / 2, concavity_copy);

        int ear_ptr;

        for(int i = 0; i < polygon_copy.size(); i++)
        {
            if(equalPoint(polygon_copy[i], ear_mon) == true )
            {
                ear_ptr = i;
                break;
            }
        }

        Point ear = polygon_copy[ear_ptr];
        Point ear_pred = ear_ptr - 1 >= 0 ? polygon_copy[ear_ptr - 1] : polygon_copy.back();
        Point ear_next = ear_ptr == polygon_copy.size() - 1 ? polygon_copy.front() : polygon_copy[ear_ptr + 1];

        solution.push_back({ear_pred, ear, ear_next});


        polygon_copy.erase(polygon_copy.begin() + ear_ptr);
        concavity_copy.erase(concavity_copy.begin() + ear_ptr);
    }

    solution.push_back({polygon_copy[0], polygon_copy[1], polygon_copy[2]});

    return solution;
}
int Logic :: is_point_inside(const Point & p) {

    vector<Triangle> rez = get_triangulation();

    for(int i = 0; i < rez.size(); i++)
    {
        if(point_inside(p, rez[i]))
            return i;
    }

    return -1;
}

Logic :: Logic(const vector<Point> & polygon_received)
{
    polygon = polygon_received;
    no_vertex = polygon.size();
}

void Logic :: replace_polygon(const vector<Point> & polygon_received)
{
    polygon.clear();
    concavity.clear();
    polygon = polygon_received;
    no_vertex = polygon.size();
}

pair<Point, Point> Logic :: check_for_edges(const Point &query)
{
    for(int i = 0; i < polygon.size() - 1; i++)
    {
        if(is_on_edge(query, polygon[i], polygon[i+1]) == true)
        {
            return {polygon[i], polygon[i+1]};
        }
    }
    if(is_on_edge(query, polygon[ polygon.size() - 1 ], polygon[0]) == true)
    {
        return {polygon[ polygon.size() - 1 ], polygon[0]};
    }
    return {{0,0}, {0,0}};
}