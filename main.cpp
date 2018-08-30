#include "parser.h"
#include "routingdb.h"
#include "tree.h"
#include <string>
#include <cstdlib>
#include <vector>
#include <cfloat>
#include <queue>
#include <math.h>
#include <list>
#include <time.h>

using namespace std;

ifstream inputFile;
ofstream outputFile;
RoutingDB db;

class Grid
{
public:
    Grid()
    {
        _sourceCost = DBL_MAX;
        _totalCost = DBL_MAX;
        _pi = NULL;
        _isDone = false;
    }

    //public member function
    int getX() const { return _x; }
    int getY() const { return _y; }
    bool isDone() const { return _isDone; }
    Grid *getPi() const { return _pi; }
    double getSourceCost() const { return _sourceCost; }
    double getTotalCost() const { return _totalCost; }

    void setX(int x) { _x = x; }
    void setY(int y) { _y = y; }
    void setDone() { _isDone = true; }
    void setPi(Grid *pi) { _pi = pi; }
    void setSourceCost(double sourceCost) { _sourceCost = sourceCost; }
    //void setTotalCost(double sourceCost, double h) { _totalCost = sourceCost + h; }
    void setTotalCost(int x, int y, int tarX, int tarY){
        double h = abs(x-tarX) + abs(y-tarY);
        _totalCost = _sourceCost + h; }

private:
    //data member
    int _x, _y;
    bool _isDone;
    Grid *_pi;
    double _sourceCost, _totalCost;
};

struct cmp
{
    bool operator()(const Grid *lhs, const Grid *rhs)
    {
        return lhs->getTotalCost() > rhs->getTotalCost();
    }
};

class Path_p
{
public:
    Path_p()
    {
        _layer1 = 1;
        _layer2 = 1;
    }

    int x1;
    int y1;
    int x2;
    int y2;
    int getLayer1() { return _layer1; }
    int getLayer2() { return _layer2; }

    void setLayer_1(int layer1) { _layer1 = layer1; }
    void setLayer_2(int layer2) { _layer2 = layer2; }

private:
    int _layer1, _layer2;

};

/*class stepstore
{
public:
    int stepst;
};*/

//global variable
vector< vector<Grid> > routingMap;
vector< vector<double> > verticalEdge; //vertical edge weight
vector< vector<double> > horizontalEdge; // horizontal edge weight
vector< vector<double> > vEdge; // store num of edge
vector< vector<double> > hEdge;// store num of edge
vector< vector<float> > vCapacity;
vector< vector<float> > hCapacity;
vector<Path_p> grid_ps; // grid point store
priority_queue< Grid *, vector<Grid *>, cmp > priorityQ;
//vector<stepstore> stepstt;
//list<Grid *> priorityQ;

//functions
void relax(Grid *u, Grid *v, double w);
Grid *extracMin();
void construct(int a, int b, int c, int d, int &steps,
               int &x_1, int &y_1, int &x_2, int &y_2,
               float &eNum, vector< vector<float> > &vCapacity, vector< vector<float> > &hCapacity);
void updateWeight(int a, int b, int c, int d,
                  float &eNum, vector< vector<float> > &vCapacity, vector< vector<float> > &hCapacity);


void ShowSyntax()
{
    cout << "syntax: " << endl;
    cout << "./gr [input_file] [output_file] " << endl;
    cout << endl;

    cout << "example: " << endl; 
    cout << "./gr input/adaptec1.capo70.2d.35.50.90.gr a1.out" << endl;

    cout << endl;
}

void HandleArgument(const int argc, char** argv)
{
    if (argc < 3)
    { ShowSyntax(); exit(1); }

    int arg_no = 1;

    /* input file */
    inputFile.open(argv[arg_no], ios::in);
    if (!inputFile)
    {
	cerr << "Could not open input file: " << argv[arg_no] << endl;
	exit(1);
    }

    arg_no++;

    /* output file */
    outputFile.open(argv[arg_no], ios::out);
    if (!outputFile)
    {
	cerr << "Could not open output file: " << argv[arg_no] << endl;
	exit(1);
    }

    arg_no++;
}


int main(int argc, char** argv) 
{
    HandleArgument(argc, argv);
    clock_t start, finish;
    start = time(NULL);

    {
	/* Parser */
	cout << "[Parser]" << endl;
	Parser parser;
	parser.ReadISPD(inputFile);
    }
    cout << endl;

    /* =================================== */
    /* Show input information after parser
    cout << "RoutingDB Info: " << endl;

    cout << "..# of horizontal global tiles: "
	<< db.GetHoriGlobalTileNo() << endl;
    cout << "..# of vertical global tiles: " 
	<< db.GetVertiGlobalTileNo() << endl;
    cout << "..# of layers: " << db.GetLayerNo() << endl;

    cout << "..vertical default capacity: ";
    for (int i = 0; i < db.GetLayerNo(); i++)
    { cout << db.GetLayerVertiCapacity(i) << " "; }
    cout << endl;

    cout << "..horizontal default capacity: ";
    for (int i = 0; i < db.GetLayerNo(); i++)
    { cout << db.GetLayerHoriCapacity(i) << " "; }
    cout << endl;

    cout << "..minimum width: ";
    for (int i = 0; i < db.GetLayerNo(); i++)
    { cout << db.GetLayerMinWidth(i) << " "; }
    cout << endl;

    cout << "..minimum spacing: ";
    for (int i = 0; i < db.GetLayerNo(); i++)
    { cout << db.GetLayerMinSpacing(i) << " "; }
    cout << endl;

    cout << "..via spacing: ";
    for (int i = 0; i < db.GetLayerNo(); i++)
    { cout << db.GetLayerViaSpacing(i) << " "; }
    cout << endl;

    cout << "..chip lower left x: " << db.GetLowerLeftX() << endl;
    cout << "..chip lower left y: " << db.GetLowerLeftY() << endl;
    cout << "..global tile width: " << db.GetTileWidth() << endl;
    cout << "..global tile height: " << db.GetTileHeight() << endl;

    cout << "..# of capacity adjustment: " << db.GetCapacityAdjustNo() << endl;
    for (int i = 0; i < db.GetCapacityAdjustNo(); i++)
    {
	CapacityAdjust& ca = db.GetCapacityAdjust(i);
    cout << "...." << ca.GetGx1() << " " << ca.GetGy1() << " " << ca.GetLayer1()
	    << " " << ca.GetGx2() << " " << ca.GetGy2() << " " << ca.GetLayer2() 
        << " " << ca.GetReduceCapacity() << endl;
    }

    cout << endl;

    cout << "..# of net: " << db.GetNetNo() << endl;

    for (int i = 0; i < db.GetNetNo(); i++)
    {
	Net& n = db.GetNetByPosition(i);

    cout << "..net[" << n.GetUid() << "] " << n.GetName() << endl;
    cout << "...# of pin: " << n.GetPinNo() << endl;
    for (int i = 0; i < n.GetPinNo(); i++)
	{
	    Pin& p = n.GetPin(i);
        cout << "....";
        p.ShowInfo();
    }

	cout << endl;

    }

    cout << endl;
     =================================== */
    /* edge whight initialize*/
    int boundaryX = db.GetHoriGlobalTileNo();
    int boundaryY = db.GetVertiGlobalTileNo();

    hEdge.resize(boundaryX);
    horizontalEdge.resize(boundaryX);
    hCapacity.resize(boundaryX);
    for(int i = 0; i < boundaryX; ++i)
    {
        horizontalEdge[i].resize(boundaryY - 1);
        hEdge[i].resize(boundaryY - 1);
        hCapacity[i].resize(boundaryY - 1);
    }
    for(int i = 0; i < boundaryX; ++i)
    {
        for(int j = 0; j < boundaryY - 1; ++j)
        {
            horizontalEdge[i][j] = 1;
            hEdge[i][j] = 0;
            hCapacity[i][j] = db.GetLayerHoriCapacity(0);
        }
    }

    vEdge.resize(boundaryX - 1);
    verticalEdge.resize(boundaryX - 1);
    vCapacity.resize(boundaryX - 1);
    for(int i = 0; i < boundaryX - 1; ++i)
    {
        verticalEdge[i].resize(boundaryY);
        vEdge[i].resize(boundaryY);
        vCapacity[i].resize(boundaryY);
    }
    for(int i = 0; i < boundaryX - 1; ++i)
    {
        for(int j = 0; j < boundaryY; ++j)
        {
            verticalEdge[i][j] = 1;
            vEdge[i][j] = 0;
            vCapacity[i][j] = db.GetLayerVertiCapacity(1);
        }
    }
    for (int i = 0; i < db.GetCapacityAdjustNo(); i++)
    {
        CapacityAdjust& caj = db.GetCapacityAdjust(i);
        //cout << "layer: " << caj.GetLayer1() << "capacity: " << caj.GetReduceCapacity() <<endl;
        if(caj.GetLayer1() == 1)
        {
            vCapacity[caj.GetGx1()][caj.GetGy1()] = caj.GetReduceCapacity();
        }
        else if(caj.GetLayer1() == 2)
        {
            hCapacity[caj.GetGx1()][caj.GetGy1()] = caj.GetReduceCapacity();
        }
    }

    /*cout << "horitzontal whight: " << endl;
    for(int i = 0; i < boundaryX; ++i)
    {
        for(int j = 0; j < boundaryY - 1; ++j)
        {
            cout << hCapacity[i][j] << " ";
        }
        cout << endl;
    }
    cout << hCapacity[2][0] << endl;
    cout << "vertical whight: " << endl;
    for(int i = 0; i < boundaryX - 1; ++i)
    {
        for(int j = 0; j < boundaryY; ++j)
        {
            cout << vCapacity[i][j] << " ";
        }
        cout<< endl;
    }*/

    /* =================================== */
    {
    cout << "[Tree Construction (Net Decomposition)]" << endl;

	RoutingTree tree;
	tree.MinimumSpanningTreeConstruction();

    //tree.ShowInfo();
    cout << endl;
    }
	
    /* =================================== */
    /* Show net(subnet) information after net decomposition */

    //cout << "Net/SubNet Info: " << endl;

    for (int i = 0; i < db.GetNetNo(); i++)
    {
        Net& n = db.GetNetByPosition(i);

        //cout << "..net[" << n.GetUid() << "] " << n.GetName() << endl;

        //cout << "...# of subnets: " << n.GetSubNetNo() << endl;

        int steps ;
        int ab ;//store steps
        double tilewidth = db.GetTileWidth();
        double tileheight = db.GetTileHeight();
        double halfWidth = db.GetTileWidth() / 2;
        double halfHeight = db.GetTileHeight() / 2;
        int lowerX = db.GetLowerLeftX();
        int lowerY = db.GetLowerLeftY();


        for (int j = 0; j < n.GetSubNetNo(); j++)
        {
            SubNet& e = n.GetSubNet(j);
            //cout << "....";
            //e.ShowInfo();
            short sourceX = 0;
            short sourceY = 0;
            short targetX = 0;
            short targetY = 0;
            sourceX = e.GetSourcePinGx();
            sourceY = e.GetSourcePinGy();
            targetX = e.GetTargetPinGx();
            targetY = e.GetTargetPinGy();

            //cout << "num:" << n.GetSubNetNo() << endl;
            //cout << "x: " << sourceX << "y: " << sourceY << endl;

            /* initialize the routing map */
            routingMap.resize(boundaryX);
            for(int a = 0; a < boundaryX; ++a)
            {
                routingMap[a].resize(boundaryY);
            }
            for(int aa = 0; aa < boundaryX; ++aa)
            {
                for(int bb = 0; bb < boundaryY; ++bb)
                {
                    routingMap[aa][bb].setX(aa);
                    routingMap[aa][bb].setY(bb);
                }
            }


            routingMap[sourceX][sourceY].setSourceCost(0);
            //routingMap[sourceX][sourceY].setTotalCost(sourceX, sourceY, targetX, targetY);
            priorityQ.push(&routingMap[sourceX][sourceY]);
            int x_1 = 0;
            int y_1 = 0;
            int x_2 = 0;
            int y_2 = 0;
            float eNum;
            /* A-Star */
            while(!priorityQ.empty() /*&& routingMap[targetX][targetY].isDone() == false*/)
            {
                Grid *u, *v;
                double w;
                u = extracMin();
                if(u->getX() == targetX && u->getY() == targetY)break;
                routingMap[u->getX()][u->getY()].setDone();

                /*if((u->getX()+1) < boundaryX )
                {
                    v = &routingMap[u->getX()+1][u->getY()];
                    w = verticalEdge[u->getX()][u->getY()];
                    relax(u, v, w);
                    v->setTotalCost(u->getX()+1, u->getY(), targetX, targetY);
                    if(!v->isDone())
                    {
                        priorityQ.push(v);
                        //v->setDone();
                    }
                }
                if(u->getX() > 0 )
                {
                    v = &routingMap[u->getX()-1][u->getY()];
                    w = verticalEdge[u->getX()-1][u->getY()];
                    relax(u, v, w);
                    v->setTotalCost(u->getX()-1, u->getY(), targetX, targetY);
                    if(!v->isDone())
                    {
                        priorityQ.push(v);
                        //v->setDone();
                    }
                }
                if((u->getY()+1) < boundaryY )
                {
                    v = &routingMap[u->getX()][u->getY()+1];
                    w = horizontalEdge[u->getX()][u->getY()];
                    relax(u, v, w);
                    v->setTotalCost(u->getX(), u->getY()+1, targetX, targetY);
                    if(!v->isDone())
                    {
                        priorityQ.push(v);
                        //v->setDone();
                    }
                }
                if(u->getY() > 0 )
                {
                    v = &routingMap[u->getX()][u->getY()-1];
                    w = horizontalEdge[u->getX()][u->getY()-1];
                    relax(u, v, w);
                    v->setTotalCost(u->getX(), u->getY()-1, targetX, targetY);
                    if(!v->isDone())
                    {
                        priorityQ.push(v);
                        //v->setDone();
                    }
                }*/

                for(int count = 0; count <= 3; ++count)
                {
                    switch(count)
                    {
                    case 0:
                        if(u->getX()==0)break;
                        v = &routingMap[u->getX()-1][u->getY()];
                        w = verticalEdge[u->getX()-1][u->getY()];
                        relax(u, v, w);
                        v->setTotalCost(u->getX()-1, u->getY(), targetX, targetY);
                        if(!v->isDone())
                        {
                            priorityQ.push(v);
                            v->setDone();
                        }
                        break;
                    case 1:
                        if(u->getX()==boundaryX-1)break;
                        v = &routingMap[u->getX()+1][u->getY()];
                        w = verticalEdge[u->getX()][u->getY()];
                        relax(u, v, w);
                        v->setTotalCost(u->getX()+1, u->getY(), targetX, targetY);
                        if(!v->isDone())
                        {
                            priorityQ.push(v);
                            v->setDone();
                        }
                        break;
                    case 2:
                        if(u->getY()==0)break;
                        v = &routingMap[u->getX()][u->getY()-1];
                        w = horizontalEdge[u->getX()][u->getY()-1];
                        relax(u, v, w);
                        v->setTotalCost(u->getX(), u->getY()-1, targetX, targetY);
                        if(!v->isDone())
                        {
                            priorityQ.push(v);
                            v->setDone();
                        }
                        break;
                    case 3:
                        if(u->getY()==boundaryY-1)break;
                        v = &routingMap[u->getX()][u->getY()+1];
                        w = horizontalEdge[u->getX()][u->getY()];
                        relax(u, v, w);
                        v->setTotalCost(u->getX(), u->getY()+1, targetX, targetY);
                        if(!v->isDone())
                        {
                            priorityQ.push(v);
                            v->setDone();
                        }
                        break;
                     default:
                        cout << "sometihng wrong!!" << endl;
                    }
                }
            }
            construct(sourceX, sourceY, targetX, targetY, steps, x_1, y_1, x_2, y_2, eNum, vCapacity, hCapacity);//

            /*stepstore pp;
            pp.stepst = steps;
            stepstt.push_back(pp);*/

            ab = 0;
            /*for(int i = 0; i < stepstt.size(); ++i)
            {
                ab += stepstt[i].stepst;
            }*/
            if(j == 0)
            {
                ab = 0;
                //cout << n.GetName() << " " << n.GetUid() << " "  << ab << endl;
                outputFile << n.GetName() << " " << n.GetUid() << " "  << ab << endl;
            }
            for(int i = grid_ps.size()-1; i > 0; --i)// int i = 0; i < grid_ps.size(); ++i      int i = grid_ps.size()-1; i >= 0; --i
            {
                /*cout << "(" <<grid_ps[i].x1*halfWidth + (grid_ps[i].x1+1)*halfWidth << "," << grid_ps[i].y1*halfHeight + (grid_ps[i].y1+1)*halfHeight << ","<< grid_ps[i].getLayer1() << ")";
                //if(i>=0)
                cout<<"-("<< grid_ps[i-1].x1*halfWidth + (grid_ps[i-1].x1+1)*halfWidth << "," << grid_ps[i-1].y1*halfHeight + (grid_ps[i-1].y1+1)*halfHeight << ","<< grid_ps[i-1].getLayer1() << ")"<<endl;
                outputFile << "(" << (grid_ps[i].x1*halfWidth + (grid_ps[i].x1+1)*halfWidth)+lowerX << "," << (grid_ps[i].y1*halfHeight + (grid_ps[i].y1+1)*halfHeight)+lowerY << ","<< grid_ps[i].getLayer1() << ")";
                //if(i>=0)
                outputFile <<"-("<< (grid_ps[i-1].x1*halfWidth + (grid_ps[i-1].x1+1)*halfWidth)+lowerX << "," << (grid_ps[i-1].y1*halfHeight + (grid_ps[i-1].y1+1)*halfHeight)+lowerY << ","<< grid_ps[i-1].getLayer1() << ")"<<endl;
                */
                //else //18 modify
                //{ print from here
                    //cout << "(" << (grid_ps[i].x1*tilewidth + halfWidth)+lowerX << "," << (grid_ps[i].y1*tileheight + halfHeight)+lowerY << ","<< grid_ps[i].getLayer1() << ")";
                    //cout<<"-("<< (grid_ps[i-1].x1*tilewidth + halfWidth)+lowerX << "," << (grid_ps[i-1].y1*tileheight + halfHeight)+lowerY << ","<< grid_ps[i-1].getLayer1() << ")"<<endl;
                    outputFile << "(" << (grid_ps[i].x1*tilewidth + halfWidth)+lowerX << "," << (grid_ps[i].y1*tileheight + halfHeight)+lowerY << ","<< grid_ps[i].getLayer1() << ")";
                    outputFile <<"-("<< (grid_ps[i-1].x1*tilewidth + halfWidth)+lowerX << "," << (grid_ps[i-1].y1*tileheight + halfHeight)+lowerY << ","<< grid_ps[i-1].getLayer1() << ")"<<endl;
                //}
            }

            grid_ps.clear();
            steps = 0;
            while(!priorityQ.empty())
            {
                priorityQ.pop();
            }
            routingMap.clear();
        }
        //cout << n.GetName() << " " << n.GetUid() << " "  << ab << endl;
        //outputFile << n.GetName() << " " << n.GetUid() << " "  << ab << endl;
        /*for(int ii = 0; ii < boundaryX; ++ii)
        {
            for(int jj = 0; jj < boundaryY-1; ++jj)
            {
                cout << horizontalEdge[ii][jj] << " " << " " << " ";
            }
            cout << endl;
        }
        for(int ii = 0; ii < boundaryX-1; ++ii)
        {
            for(int jj = 0; jj < boundaryY; ++jj)
            {
                cout << verticalEdge[ii][jj] << " " << " " << " ";
            }
            cout << endl;
        }*/

        /*for(int i = grid_ps.size()-1; i > 0; --i)// int i = 0; i < grid_ps.size(); ++i      int i = grid_ps.size()-1; i >= 0; --i
        {
            if(grid_ps[i].x1 != grid_ps[i-1].x1 && grid_ps[i].y1 != grid_ps[i-1].y1)
            {
                cout;
                outputFile;
            }
            //else
            //{
                cout << "(" << (grid_ps[i].x1*halfWidth + (grid_ps[i].x1+1)*halfWidth)+lowerX << "," << (grid_ps[i].y1*halfHeight + (grid_ps[i].y1+1)*halfHeight)+lowerY << ","<< grid_ps[i].getLayer1() << ")";
                //if(i>=0)
                cout<<"-("<< (grid_ps[i-1].x1*halfWidth + (grid_ps[i-1].x1+1)*halfWidth)+lowerX << "," << (grid_ps[i-1].y1*halfHeight + (grid_ps[i-1].y1+1)*halfHeight)+lowerY << ","<< grid_ps[i-1].getLayer1() << ")"<<endl;
                outputFile << "(" << (grid_ps[i].x1*halfWidth + (grid_ps[i].x1+1)*halfWidth)+lowerX << "," << (grid_ps[i].y1*halfHeight + (grid_ps[i].y1+1)*halfHeight)+lowerY << ","<< grid_ps[i].getLayer1() << ")";
                //if(i>=0)
                outputFile <<"-("<< (grid_ps[i-1].x1*halfWidth + (grid_ps[i-1].x1+1)*halfWidth)+lowerX << "," << (grid_ps[i-1].y1*halfHeight + (grid_ps[i-1].y1+1)*halfHeight)+lowerY << ","<< grid_ps[i-1].getLayer1() << ")"<<endl;
            //}
            else //18 modify
            {
                cout << "(" << (grid_ps[i].x1*tilewidth + halfWidth)+lowerX << "," << (grid_ps[i].y1*tileheight + halfHeight)+lowerY << ","<< grid_ps[i].getLayer1() << ")";
                //if(i>=0)
                cout<<"-("<< (grid_ps[i-1].x1*tilewidth + halfWidth)+lowerX << "," << (grid_ps[i-1].y1*tileheight + halfHeight)+lowerY << ","<< grid_ps[i-1].getLayer1() << ")"<<endl;
                outputFile << "(" << (grid_ps[i].x1*tilewidth + halfWidth)+lowerX << "," << (grid_ps[i].y1*tileheight + halfHeight)+lowerY << ","<< grid_ps[i].getLayer1() << ")";
                //if(i>=0)
                outputFile <<"-("<< (grid_ps[i-1].x1*tilewidth + halfWidth)+lowerX << "," << (grid_ps[i-1].y1*tileheight + halfHeight)+lowerY << ","<< grid_ps[i-1].getLayer1() << ")"<<endl;
            }

        }*/

        //grid_ps.clear();
        //routingMap.clear();
        //stepstt.clear();
        //cout << "!" << endl;
        outputFile << "!" << endl;
        //cout << endl;
    }

    //cout << endl;
    /* =================================== */


    {
    finish = time(NULL);
    cout << "runtime is = " << (double)(finish - start) << endl;

	cout << "[Verify]" << endl;
	char cmd[100];

    //sprintf(cmd, "./eval2008.pl %s %s", argv[1], argv[2]);
    sprintf(cmd, "./eval.pl %s %s", argv[1], argv[2]);
	cout << cmd << endl;
	system(cmd);
    }

    cout << endl;
    cout << endl;

    return 0;
}

void relax(Grid *u, Grid *v, double w)
{
    //double h = abs(targetX-(v->getX())) + abs(targetY-(v->getY()));
    if(v->getSourceCost() > u->getSourceCost()+w )
    {
        v->setSourceCost(u->getSourceCost()+w);
        v->setPi(u);
    }
    //v->setTotalCost(v->getSourceCost(), h);
}

Grid *extracMin()
{
    Grid *minGrid = priorityQ.top();
    priorityQ.pop();
    return minGrid;
}

void construct(int a, int b, int c, int d, int &steps, int &x_1, int &y_1, int &x_2, int &y_2, float &eNum, vector< vector<float> > &vCapacity, vector< vector<float> > &hCapacity)//
{
    Grid *t, *m;
    Path_p pstore;
    t = &routingMap[c][d];
    //bool layerTemp = true;
    //test
    pstore.x1 = t->getX();
    pstore.y1 = t->getY();
    pstore.setLayer_1(1);
    grid_ps.push_back(pstore);
    while(!(t->getX()== routingMap[a][b].getX() && t->getY()== routingMap[a][b].getY())) //t != &routingmap[a][b] !(t->getX()== routingMap[a][b].getX() && t->getY()== routingMap[a][b].getY())
    {
            m = t->getPi();
            if(m->getY() != t->getY() && m->getX() == t->getX()) //go vertical
            {
                  if(grid_ps[grid_ps.size()-1].getLayer1()==1) //if layer=1 before
                  {
                      pstore.x1 = t->getX();
                      pstore.y1 = t->getY();
                      pstore.setLayer_1(2);
                      grid_ps.push_back(pstore);
                      ++steps;
                      x_1 = t->getX();
                      y_1 = t->getY();
                      x_2 = t->getX();
                      y_2 = t->getY();
                      updateWeight(x_1, y_1, x_2, y_2, eNum, vCapacity, hCapacity);//, vCapacity, hCapacity
                  }
                  pstore.x1 = m->getX();
                  pstore.y1 = m->getY();
                  pstore.setLayer_1(2);
                  grid_ps.push_back(pstore);
                  x_1 = t->getX();
                  y_1 = t->getY();
                  x_2 = m->getX();
                  y_2 = m->getY();
                  updateWeight(x_1, y_1, x_2, y_2, eNum, vCapacity, hCapacity);
            }
            else if(m->getX() != t->getX() && m->getY() == t->getY()) //go horizontal
            {
                if(grid_ps[grid_ps.size()-1].getLayer1()==2)
                {
                     pstore.x1 = t->getX();
                     pstore.y1 = t->getY();
                     pstore.setLayer_1(1);
                     grid_ps.push_back(pstore);
                     ++steps;
                     x_1 = t->getX();
                     y_1 = t->getY();
                     x_2 = t->getX();
                     y_2 = t->getY();
                     updateWeight(x_1, y_1, x_2, y_2, eNum, vCapacity, hCapacity);
                }
                pstore.x1 = m->getX();
                pstore.y1 = m->getY();
                pstore.setLayer_1(1);
                grid_ps.push_back(pstore);
                x_1 = t->getX();
                y_1 = t->getY();
                x_2 = m->getX();
                y_2 = m->getY();
                updateWeight(x_1, y_1, x_2, y_2, eNum, vCapacity, hCapacity);
            }

            // for weight update
            /*x_1 = t->getX();
            y_1 = t->getY();
            x_2 = m->getX();
            y_2 = m->getY();
            updateWeight(x_1, y_1, x_2, y_2, eNum, vCapacity, hCapacity);*/
            //grid_ps.push_back(pstore);
            t = m;
            ++steps;
    }
    if(t->getX()== routingMap[a][b].getX() && t->getY()== routingMap[a][b].getY())
    {
        if((grid_ps[grid_ps.size()-1].x1 == grid_ps[grid_ps.size()-2].x1) && (grid_ps[grid_ps.size()-1].y1 == grid_ps[grid_ps.size()-2].y1))
        {
            grid_ps[grid_ps.size()].setLayer_1(1);
        }
        else if(grid_ps[grid_ps.size()-1].getLayer1() == 2)
        {
            pstore.x1 = routingMap[a][b].getX();
            pstore.y1 = routingMap[a][b].getY();
            pstore.setLayer_1(1);
            grid_ps.push_back(pstore);
            /*pstore.x1 = routingMap[a][b].getX();
            pstore.y1 = routingMap[a][b].getY();
            pstore.setLayer_1(1);
            grid_ps.push_back(pstore);*/
        }
    }

#if 0
    while(!( t->getX()== routingMap[a][b].getX() && t->getY()== routingMap[a][b].getY()) ) //t != &routingMap[a][b]
    {
        /*m = t->getPi();
        pstore.x1 = m->getX();
        pstore.y1 = m->getY();
        pstore.x2 = t->getX();
        pstore.y2 = t->getY();

        // for weight update
        x_1 = t->getX();
        y_1 = t->getY();
        x_2 = m->getX();
        y_2 = m->getY();
        updateWeight(x_1, y_1, x_2, y_2, eNum, vCapacity, hCapacity);
        grid_ps.push_back(pstore);
        t = m;
        ++steps;*/
        if(m->getY()!=t->getY()) //go vertical
                {
                      if(grid_ps[grid_ps.size()-1].getLayer1()==1) //if layer=1 before
                      {
                          pstore.x1 = t->getX();
                          pstore.y1 = t->getY();
                          pstore.setLayer_1(2);
                          grid_ps.push_back(pstore);
                      }
                      pstore.x1 = m->getX();
                      pstore.y1 = m->getY();
                      pstore.setLayer_1(2);
                      grid_ps.push_back(pstore);
                }
                else                    //go horizontal
                {
                    if(grid_ps[grid_ps.size()-1].getLayer1()==2)
                    {
                         pstore.x1 = t->getX();
                         pstore.y1 = t->getY();
                         pstore.setLayer_1(1);
                         grid_ps.push_back(pstore);
                    }
                    pstore.x1 = m->getX();
                    pstore.y1 = m->getY();
                    pstore.setLayer_1(1);
                    grid_ps.push_back(pstore);
                }

                // for weight update
                x_1 = t->getX();
                y_1 = t->getY();
                x_2 = m->getX();
                y_2 = m->getY();
                updateWeight(x_1, y_1, x_2, y_2, eNum, vCapacity, hCapacity);
                //grid_ps.push_back(pstore);
                t = m;
                ++steps;

    }
#endif

}

void updateWeight(int a, int b, int c, int d, float &eNum, vector<vector<float> > &vCapacity, vector<vector<float> > &hCapacity)
{
    if(a < c)
    {
        if(vCapacity[a][b] == 0)
        {
            //eNum = 1000;
            verticalEdge[a][b] = 9999;
        }
        else
        {
            vEdge[a][b] += 1;// b vv
            eNum = vEdge[a][b];
            /*if(eNum >= (vCapacity[a][b]/2))
            {
                verticalEdge[a][b] = 9999;
            }*/
            //else
            //{
                verticalEdge[a][b] += pow(2.0, eNum / (vCapacity[a][b] / 2)) - 1.0;
            //}
        }
    }
    else if(a == c)
    {
        ;
    }
    if(c < a)
    {
        if(vCapacity[c][b] == 0)
        {
            //eNum = 1000;
            verticalEdge[c][b] = 9999;
        }
        else
        {
            vEdge[c][b] += 1;//b vv
            eNum = vEdge[c][b];
            /*if(eNum >= (vCapacity[c][b]/2))
            {
                verticalEdge[c][b] = 9999;
            }*/
            //else
            //{
                verticalEdge[c][b] += pow(2.0, eNum / (vCapacity[c][b] / 2)) - 1.0;
            //}
        }
    }
    else if(c == a)
    {
        ;
    }
    if(b < d)
    {
        if(hCapacity[a][b] == 0)
        {
            //eNum = 1000;
            horizontalEdge[a][b] = 9999;
        }
        else
        {
            hEdge[a][b] += 1;//a hh
            eNum = hEdge[a][b];
            /*if(eNum >= (hCapacity[a][b]/2))
            {
                horizontalEdge[a][b] = 9999;
            }*/
            //else
            //{
                horizontalEdge[a][b] += pow(2.0, eNum / (hCapacity[a][b] / 2)) - 1.0;
            //}
        }
    }
    else if(b == d)
    {
        ;
    }
    if(d < b)
    {
        if(hCapacity[a][d] == 0)
        {
            //eNum = 1000;
            horizontalEdge[a][d] = 9999;
        }
        else
        {
            hEdge[a][d] += 1;//a hh
            eNum = hEdge[a][d];
            /*if(eNum >= (hCapacity[a][d]/2))
            {
                horizontalEdge[a][d] = 9999;
            }*/
            //else
            //{
                horizontalEdge[a][d] += pow(2.0, eNum / (hCapacity[a][d] / 2)) - 1.0;
            //}
        }
    }
    else if(d == b)
    {
        ;
    }
}

