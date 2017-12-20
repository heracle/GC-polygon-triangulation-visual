#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>
#include <unistd.h>
using namespace std;

const int WIDTH = 1280;
const int HEIGHT = 720;

struct Point {
    double x, y;
};

struct Triangle {
    Point A, B, C;
};

class Logic {
private:
    enum{
        convex = 0,
        concave = 1
    };

    vector<bool> concavity;
    vector<Point> polygon;
    int no_vertex;

    // double hash1, hash2;


    const static double epsilon;
    const static double near_edge;


    bool trigonometric_order(const Point &A, const Point &B, const Point &C)
    {
        if( (A.x - B.x) * (A.y - C.y) - (A.y - B.y) * (A.x - C.x) <= 0)
            return true;
        else
            return false;
    }

    bool point_inside(const Point & query, const Triangle & T)
    {
        bool t1, t2, t3;

        t1 = trigonometric_order(query, T.A, T.B);
        t2 = trigonometric_order(query, T.B, T.C);
        t3 = trigonometric_order(query, T.C, T.A);

        if(t1 == t2 && t1 == t3)
            return true;
        return false;
    }

    bool check_for_ear(const vector<Point> & polygon_remote, const int & position, const vector<bool> & concavity_remote)
    {
        //cerr << "concavity check for ear:\n";

        for(int i = 0; i < concavity_remote.size(); i++)
        {
            //cerr << concavity_remote[i] << " ";
        }

        //cerr << "\n";

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

    bool equal(const double &a, const double &b)
    {
        double dif = a - b;

        if(dif < 0)
            dif = - dif;

        return dif < epsilon;
    }

    bool equalPoint(const Point &a, const Point &b)
    {
        return (equal(a.x, b.x) && equal(a.y, b.y));
    }

    Point ask_for_a_ray(const Point &act, const Point & fst, const Point & lst)
    {

        ////cerr << act.x << " " << act.y <<" # " << fst.x << " " << fst.y << " ## " << lst.x << " " << lst.y << endl;

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

    void rotate_polygon(vector<Point> & polygon_remote, double angle)
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

    double get_angle(const Point &A, const Point &B, const Point &C)
    {
        double angle = atan2(A.y - C.y, A.x - C.x);
        if(angle < 0)
            return -angle;
        return angle;
    }


    int ask_for_second_position(const Point &Y, const Point &Pi, const Point &Pk, vector<Point> & polygon_remote, int my_k, const Point &pred)
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

        //cerr << R.size() << " size de R" << endl;

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

    void ask_for_a_polygon(vector<Point> & polygon_remote, int a, int b, vector<bool> & concavity_remote)
    {
 //       cout << a << " " << b << "intervalul din ask_for_pynom\n";
        if(b < a)
        {
            swap(a, b);
        }

        vector<Point> new_polygon;
        vector<bool> new_concavity;

        // if(b - a < polygon_remote.size() / 2)
        // {
            for(int i = a; i <= b; i++)
            {
                new_polygon.push_back(polygon_remote[i]);
                new_concavity.push_back(concavity_remote[i]);
            }
        // }
        // else
        // {
        //     for(int i = a; i >= 0; i--)
        //     {
        //         new_polygon.push_back(polygon_remote[i]);
        //         new_concavity.push_back(concavity_remote[i]);
        //     }
        //     for(int i = polygon_remote.size() - 1; i >= b ; i-- )
        //     {
        //         new_polygon.push_back(polygon_remote[i]);
        //         new_concavity.push_back(concavity_remote[i]);
        //     }
        // }

        polygon_remote.clear();
        polygon_remote = new_polygon;

        concavity_remote.clear();
        concavity_remote = new_concavity;
    }

    // double make_hash1(const Point &a, const Point &b, const Point &c)
    // {
    //     return a.x * a.y + b.x * b.y + c.x * c.y;
    // }


    // double make_hash2(const Point &a, const Point &b, const Point &c)
    // {
    //     return (a.x + a.y) * (b.x + b.y) * (c.x + c.y);
    // }

    Point FindAnEar(vector<Point> polygon_remote, const int & position, vector<bool> concavity_remote)
    {
        //cerr << position << " :  " << polygon_remote[position].x << " " << polygon_remote[position].y << "ce vreau\n";

        for(int i = 0; i < polygon_remote.size(); i++)
        {
            //cerr << polygon_remote[i].x << " " << polygon_remote[i].y << ";  ";
        }
        //cerr << " lista de puncte actuale\n";

        if( polygon_remote.size() < 7)
        {
            //cerr << "aici am fost\n";
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

        //make smen


        int pred, next;


        if(position == 0)
            pred = polygon_remote.size() - 1;
        else
            pred = position - 1;

        if(position == polygon_remote.size() - 1)
            next = 0;
        else
            next = position + 1;



        //rotate plane


        Point asoc = {(polygon_remote[pred].x + polygon_remote[next].x) / 2, (polygon_remote[pred].y + polygon_remote[next].y) / 2};

        // //cerr << asoc.x << " " << asoc.y << " asoc  pct "<< act.x << " " << act.y << "\n";

        double angle = atan2(asoc.y - polygon_remote[position].y, asoc.x - polygon_remote[position].x);

        double pi = acos(0);
        pi *= 2;


        if(concavity_remote[position] == concave)
        {
            angle += pi;
        }

        angle += pi / 2; 
        angle = 2 * pi - angle;



        // //cerr << angle << " before"<<endl;
        // for(int i = 0; i < polygon_remote.size(); i++)
        // {
        //     //cerr << polygon_remote[i].x << " " << polygon_remote[i].y << "\n";
        // }


        rotate_polygon(polygon_remote, angle);

        for(int i = 0; i < polygon_remote.size(); i++)
        {
            //cerr << polygon_remote[i].x << " " << polygon_remote[i].y << ";  ";
        }
        //cerr << " lista de puncte dupa rotire\n";


        Point act = polygon_remote[position];


        // //cerr << "after\n";
        // for(int i = 0; i < polygon_remote.size(); i++)
        // {
        //     //cerr << polygon_remote[i].x << " " << polygon_remote[i].y << "\n";
        // }
    



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

             //cerr << fst.x << " " << fst.y <<" + " << lst.x << " " << lst.y << " doua puncte poligon --> y-ul gasit " << needed_y.x << " " << needed_y.y << "\n";

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

        //cerr << proper_k <<"acel k\n";

        assert (equalPoint(proper_y, polygon_remote[position]) == 0 );

        for(int i = 0; i < polygon_remote.size(); i++)
        {
            if(equalPoint(proper_y, polygon_remote[i]))
            {
               // cout << "aici? " << i <<" cu el\n";
                //cerr << "marea coincidenta\n";

                ask_for_a_polygon(polygon_remote, position, i, concavity_remote);


                rotate_polygon(polygon_remote, 2 * pi - angle);

                
                for(int i = 0; i < polygon_remote.size(); i++)
                {
                    //cerr << polygon_remote[i].x << " " << polygon_remote[i].y << ";  ";
                }
                //cerr << " lista de puncte actuale\n";

                if(polygon_remote.size() == 3)
                {
                    //cerr << polygon_remote[0].x << " " << polygon_remote[0].y <<" - " << polygon_remote[1].x << " " << polygon_remote[1].y <<" - " << polygon_remote[2].x << " " << polygon_remote[2].y <<" \n";

                    // hash1 = make_hash1(polygon_remote[0], polygon_remote[1], polygon_remote[2]);
                    // hash2 = make_hash2(polygon_remote[0], polygon_remote[1], polygon_remote[2]);
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

        //cerr << second_position << " raspuns dupa prima incercare\n";

        if(second_position >= 0 )
        {
            ask_for_a_polygon(polygon_remote, position, second_position, concavity_remote);
            rotate_polygon(polygon_remote, 2 * pi - angle);

            
            for(int i = 0; i < polygon_remote.size(); i++)
            {
                //cerr << polygon_remote[i].x << " " << polygon_remote[i].y << ";  ";
            }
            //cerr << " lista de puncte actuale\n";


            if(polygon_remote.size() == 3)
            {

                    //cerr << polygon_remote[0].x << " " << polygon_remote[0].y <<" - " << polygon_remote[1].x << " " << polygon_remote[1].y <<" - " << polygon_remote[2].x << " " << polygon_remote[2].y <<" \n";
                // hash1 = make_hash1(polygon_remote[0], polygon_remote[1], polygon_remote[2]);
                // hash2 = make_hash2(polygon_remote[0], polygon_remote[1], polygon_remote[2]);
                return polygon_remote[1];
            }

            return FindAnEar(polygon_remote, polygon_remote.size() / 2, concavity_remote);  
        }

        //cerr << "my Y: "<< proper_y.x << " " << proper_y.y << endl;

        //cerr << proper_k << "=proper_k " << next <<"=next " << endl;

        second_position = ask_for_second_position(proper_y, act, polygon_remote[proper_k], polygon_remote, proper_k, polygon_remote[next]);

        //cerr << second_position << " raspuns dupa a doua incercare\n";


        if(second_position >= 0)
        {
            ask_for_a_polygon(polygon_remote, position, second_position, concavity_remote);
            rotate_polygon(polygon_remote, 2 * pi - angle);


            
            for(int i = 0; i < polygon_remote.size(); i++)
            {
                //cerr << polygon_remote[i].x << " " << polygon_remote[i].y << ";  ";
            }
            //cerr << " lista de puncte actuale\n";

            if(polygon_remote.size() == 3)
            {

                //     //cerr << polygon_remote[0].x << " " << polygon_remote[0].y <<" - " << polygon_remote[1].x << " " << polygon_remote[1].y <<" - " << polygon_remote[2].x << " " << polygon_remote[2].y <<" \n";
                // hash1 = make_hash1(polygon_remote[0], polygon_remote[1], polygon_remote[2]);
                // hash2 = make_hash2(polygon_remote[0], polygon_remote[1], polygon_remote[2]);
                return polygon_remote[1];
            }

            return FindAnEar(polygon_remote, polygon_remote.size() / 2, concavity_remote);  
        } 

        throw "belea";

        assert(false);
        
    }

    void dump(vector<Point> polygon)
    {
        for(int i = 0; i < polygon.size(); i++)
        {
            printf("%.5lf %.5lf\n", polygon[i].x, polygon[i].y);
        }

        printf("\n\n\n");
    }

    void sort_points_by_convexity(vector<Point> & polygon, vector<bool> & concavity)
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
                //cerr << i << " inside\n";
            }
        }

        if(trigonometric_order(polygon[polygon.size() - 2], polygon[polygon.size() - 1], polygon[0]) == true)
        {
            trigo_list.push_back(polygon.size() - 1);

            //cerr << polygon.size() - 1 << " inside\n";
        }
        if(trigonometric_order(polygon[polygon.size() - 1], polygon[0], polygon[1]) == true)
        {
            trigo_list.push_back(0);
            //cerr << 0 << " inside\n";
        }
    
        //cerr << trigo_list.size() << "=trigo list\n";

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

        //cerr << "concavity right after calculation:\n";

        for(int i = 0; i < concavity.size(); i++)
        {
            //cerr << concavity[i] << " ";
        }

        //cerr << "\n";
    }

    Point middle(const Point &a, const Point &b, const Point &c)
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
    }

    double distance(const Point &a, const Point &b)
    {
        return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
    }

    bool is_on_edge(const Point &query, const Point &A, const Point &B)
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

public:
    vector<Triangle> get_triangulation() {

        sort_points_by_convexity(polygon, concavity);//OK

        // for(int i = 0; i < no_vertex; i++)
        // {
        //     //cerr << concavity[i] << " ";
        // }

        vector<Triangle> solution;

        if(polygon.size() == 3)
        {
            solution.push_back({polygon[0], polygon[1], polygon[2]});
            return solution;
        }

        if(polygon.size() < 3)
        {
            //cerr << "prea putine puncte";
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

            // if(equal(hash1, -1) == 0 || equal(hash2, -1) == 0 )
            // {
            //     if(ear_ptr == 0)
            //         ear_ptr = polygon_copy.size() - 1;
            //     else
            //         ear_ptr--;

            //     //cerr << hash1 << " " << hash2 << endl;

            //     Label:

            //     cout << ear_ptr << endl;

            //     Point ear = polygon_copy[ear_ptr];
            //     Point ear_pred = ear_ptr - 1 >= 0 ? polygon_copy[ear_ptr - 1] : polygon_copy.back();
            //     Point ear_next = ear_ptr == polygon_copy.size() - 1 ? polygon_copy.front() : polygon_copy[ear_ptr + 1];

            //      cout << ear.x << " " << ear.y <<" - " << ear_pred.x << " " << ear_pred.y <<" - " << ear_next.x << " " << ear_next.y <<" \n";
               

            //    cout << hash1 << " " << make_hash1(ear_pred, ear, ear_next) << " -- " << hash2 << " " << make_hash2(ear_pred, ear, ear_next) << "\n";

            //     if(equal(hash1, make_hash1(ear_pred, ear, ear_next)) == 0 || equal(hash2, make_hash2(ear_pred, ear, ear_next)) == 0)
            //     {
            //         if(ear_ptr == polygon_copy.size() - 1)
            //             ear_ptr = 0;
            //         else
            //             ear_ptr ++;
            //         goto Label;
            //     }


            // }

            //cerr << polygon_copy[ear_ptr].x << " " << polygon_copy[ear_ptr].y << "   ear ptr: " << ear_ptr <<  " ureche returnata\n\n\n";

            Point ear = polygon_copy[ear_ptr];
            Point ear_pred = ear_ptr - 1 >= 0 ? polygon_copy[ear_ptr - 1] : polygon_copy.back();
            Point ear_next = ear_ptr == polygon_copy.size() - 1 ? polygon_copy.front() : polygon_copy[ear_ptr + 1];

            solution.push_back({ear_pred, ear, ear_next});


            polygon_copy.erase(polygon_copy.begin() + ear_ptr);
            concavity_copy.erase(concavity_copy.begin() + ear_ptr);


            //dump(polygon_copy);
        }

        solution.push_back({polygon_copy[0], polygon_copy[1], polygon_copy[2]});

        return solution;
    }
    int is_point_inside(const Point & p) {

        vector<Triangle> rez = get_triangulation();

        for(int i = 0; i < rez.size(); i++)
        {
            if(point_inside(p, rez[i]))
                return i;
        }

        //todo on edge

        return -1;
    }

    Logic(const vector<Point> & polygon_received)
    {
        polygon = polygon_received;
        no_vertex = polygon.size();
    }

    void replace_polygon(const vector<Point> & polygon_received)
    {
        polygon.clear();
        concavity.clear();
        polygon = polygon_received;
        no_vertex = polygon.size();
    }

    public:

    pair<Point, Point> check_for_edges(const Point &query)
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
};

const double Logic::epsilon = 0.0000001;
const double Logic::near_edge = 10.0;

class Renderer {
    const float circleShapeSize = 6.f;
    private:
    sf::RenderWindow & window;
    vector<sf::CircleShape> points;
    sf::CircleShape externPoint;

    sf::Vector2i getMousePosition() {
        return sf::Mouse::getPosition(window);
    }

    void init_polygon(sf::ConvexShape& polygon) {
        polygon.setFillColor(sf::Color::Transparent);
        polygon.setOutlineColor(sf::Color::Red);
        polygon.setOutlineThickness(3.0f);
        polygon.setPointCount(0);
        externPoint.setFillColor(sf::Color::Magenta);
    }
    void init_triang(sf::ConvexShape& polygon) {
        polygon.setFillColor(sf::Color(100, 100, 100));
        polygon.setOutlineColor(sf::Color::Transparent);
        polygon.setOutlineThickness(3.0f);
        polygon.setPointCount(0);
    }
    void handleClick() {
        auto position = getMousePosition();
        if (position.x <= 0
        || position.y <= 0
        || position.x >= WIDTH
        || position.y >= HEIGHT) {
            return;
        }
        sf::CircleShape point(Renderer::circleShapeSize);
        point.setPosition(
            position.x - Renderer::circleShapeSize,
            position.y - Renderer::circleShapeSize);
        point.setFillColor(sf::Color::Red);
        points.push_back(point);
        // clicked at position.x position.y
    }
    void handleRightClick() {
        auto position = getMousePosition();
        // maybe Vector2f for floats
        // to move test point at new location
        externPoint.setPosition(sf::Vector2f(
            position.x - 1.5 * Renderer::circleShapeSize,
            position.y - 1.5 * Renderer::circleShapeSize
        ));
    }
    void clear() {
        points.clear();
    }
    void update() {
        window.clear();

        sf::ConvexShape polygon;
        init_polygon(polygon);
        polygon.setPointCount(points.size());
        int index = 0;

        for (auto point : points) {
            auto position = point.getPosition();
            polygon.setPoint(index, {
                position.x + Renderer::circleShapeSize,
                position.y + Renderer::circleShapeSize});
            index ++;
        }

        window.draw(polygon);

        for (auto point : points) {
            window.draw(point);
        }

        vector<Point> discrete_polygon;
        for (auto p : points) {
            discrete_polygon.push_back(
                { p.getPosition().x, p.getPosition().y }
            );
        }

        Logic tester1(discrete_polygon);
        Logic tester2(discrete_polygon);
        vector<Triangle> solution;
        if(discrete_polygon.size() > 2) {
            try {
                solution = tester1.get_triangulation();
            } catch (int w) { }
        }

        auto drawLine = [this](Point a, Point b, int col = 1) {
            sf::Vertex line[] = {
                sf::Vertex({ 
                    (float) a.x + Renderer::circleShapeSize,
                    (float) a.y + Renderer::circleShapeSize }),
                sf::Vertex({
                    (float) b.x + Renderer::circleShapeSize,
                    (float) b.y + Renderer::circleShapeSize })
            };
            if(col == 1) {
                line[0].color  = sf::Color::Green;
                line[1].color  = sf::Color::Green;
            } else {
                line[0].color  = sf::Color::Yellow;
                line[1].color  = sf::Color::Yellow;
            }
            window.draw(line, 2, sf::Lines);
        };

        for (auto it : solution) {
            drawLine(it.A, it.B);
            drawLine(it.B, it.C);
            drawLine(it.C, it.A);
        }
        
        Point extern_pos = {
            externPoint.getPosition().x + 1.5 * Renderer::circleShapeSize,
            externPoint.getPosition().y + 1.5 * Renderer::circleShapeSize
        };

        int response = -1;
        if(discrete_polygon.size() > 2) {
            try {
                response = tester2.is_point_inside(extern_pos);
            } catch (int w) { }
        }

        if (response == -1) {
            externPoint.setFillColor(sf::Color::Magenta);
        } else {
            externPoint.setFillColor(sf::Color::Green);
            sf::ConvexShape triang;
            init_triang(triang);
            triang.setPointCount(3);
            Triangle t = solution[response];
            triang.setPoint(0, {
                (float) t.A.x + Renderer::circleShapeSize,
                (float) t.A.y + Renderer::circleShapeSize});
            triang.setPoint(1, {
                (float) t.B.x + Renderer::circleShapeSize,
                (float) t.B.y + Renderer::circleShapeSize});
            triang.setPoint(2, {
                (float) t.C.x + Renderer::circleShapeSize,
                (float) t.C.y + Renderer::circleShapeSize});
            window.draw(triang);
        }

        if(discrete_polygon.size() > 2) {
            auto pos_edge = tester2.check_for_edges(extern_pos);
            for(int i=1; i <= 20; i++) {
                double dx = double(rand() % 10);
                double dy = double(rand() % 10);
                drawLine(
                    {pos_edge.first.x + dx, pos_edge.first.y + dy},
                    {pos_edge.second.x + dx, pos_edge.second.y + dy},
                2);
            }
        }

        window.draw(externPoint);
        window.display();
    }
    public:
    Renderer(sf::RenderWindow & _window) :
        window(_window),
        externPoint(1.5*Renderer::circleShapeSize) { }
    
    void startRenderWindow() {
        bool painterRunning = true;
        thread painter ([this, &painterRunning]() {
            while (painterRunning) {
                update();
                this_thread::sleep_for(chrono::milliseconds(100));
            }
        });
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    painterRunning = false;
                    if(painter.joinable()) {
                        painter.join();
                    }
                    window.close();
                }
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                {
                    // method time measure
                    auto t1 = std::chrono::high_resolution_clock::now();
                    handleClick();
                    auto t2 = std::chrono::high_resolution_clock::now();
                    auto duration =
                        std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
                    this_thread::sleep_for(chrono::milliseconds(200));
                    // cout << "handleClickDuration: " << duration << '\n';
                }
                if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
                {
                    // click dreapta muta punctul de test "externPoint"
                    handleRightClick();
                }
                //cout << "reached control" << '\n';
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
                {
                    // stergere poligon
                    clear();
                }
            }
        }
        painterRunning = false;
        if(painter.joinable()) {
            painter.join();
        }
    }
};

int main() {
    sf::ContextSettings contextSettings;
    contextSettings.antialiasingLevel = 0;
    sf::VideoMode videoMode(WIDTH, HEIGHT);
    sf::RenderWindow window(videoMode, "Proiect GC", 
        sf::Style::Default, contextSettings);
    Renderer renderer(window);
    renderer.startRenderWindow();
}