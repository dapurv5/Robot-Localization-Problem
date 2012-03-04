//============================================================================
// Name        : RobotLocalization.cpp
// Author      : Apurv Verma, Prateek Garg, Kumar Ashwini
// Version     :
// Copyright   : Undergraduate Term Project 2011-2012
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "constants.h"
#include "MapIOHandler.h"
#include "PolygonUtil.h"
#include "HypothesisGenerator.h"
#include "Majoritymap.h"
#include "UIutil.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <hash_map>


#define WIDTH 840
#define HEIGHT 680

using namespace std;

 string sh1,sh2;
 string shyp1,shyp2; //live variable
 int mapID=1;
 float fxcoordinate,fycoordinate;
 GLUI_Panel *FijPanel;
 bool flagForKi=false;

 float zoomfactor=1.0;
 Camera camera;

 GLUI_Spinner *edgeSpinner;
 GLUI_Listbox *mapList;

 /*
  * for Fij's
  */
 list<Majoritymap> Kijs;
 list<list<Polygon> > Gijs;
 list<list<Polygon> > Fijs;
 list<list<list<Polygon> > > all_two_faces;


map<int,Segment> EdgeMap;
map<int,string> mapMap;  //mapping from integer to map for changing the map in the gui


/*
 * this integer is used to decide the phase of the algorithm to be displayed in the glui Window.
 * phase=1  Displaying the Map
 * phase=2 Displaying the Visibility Polygon
 * phase =3 Displaying the Set of Hypothesis
 */

int phase=0;
Majoritymap mmap;
list<Polygon> polygonList; //for facecontaining origin
// initialising the polygons
//----------------------------------------------------------------------------------------------

PolygonUtil pUtil		= PolygonUtil();
Polygon mapP;
Polygon visP;
Point robotPos;
Segment seg;
HypothesisGenerator g;
list<Point> hyps;  //list of hypothesis
int no_of_hypothesis;


//-----------------------------------------------------------------------------------------------

void DisplayMajorityMap();
void DisplayFaceContainingOrigin();
void DisplayVisibilityPolygonEdge();
void DisplayLocalMap();
void DisplayMajorityMap();
void MapIntegerToEdge();
void LoadNewMap();
void DisplayMap();
void ShowMap(int id);

void LoadNewMap()
{

	string s=mapMap[mapID];
	MapIOHandler handle 	= MapIOHandler(s.c_str());
	mapP	  		        = handle.GetMapPolygon();
	visP                    = handle.GetVisibilityPolygon();
	robotPos			    = handle.GetRobotPosition();
	MapIntegerToEdge();
	seg=EdgeMap[1];

	hyps.clear();
	mmap.noOfHypothesis=0;
	polygonList.clear();
	Kijs.clear();
	Gijs.clear();
	Fijs.clear();
	all_two_faces.clear();
	  edgeSpinner->set_int_limits(1,mapP.size(),GLUI_LIMIT_CLAMP);


	//	DisplayMap();
	int listIDMap=tessellate1(mapP,0);
	int listIDVPolygon=tessellate1(visP,0);
	glColor3f(1,0,0);
	glCallList(listIDMap);
	glColor3f(0,1,0);
	glCallList(listIDVPolygon);
}

void special(int key, int, int) {
  switch (key) {
    case GLUT_KEY_LEFT:  camera.moveLeft(); break;
    case GLUT_KEY_RIGHT: camera.moveRight(); break;
    case GLUT_KEY_UP:    camera.moveUp(); break;
    case GLUT_KEY_DOWN:  camera.moveDown(); break;
  }
    glutPostRedisplay();

}

void MapIntegerToEdge()
{
	int i=1;
	for(EdgeIterator ei=mapP.edges_begin();ei!=mapP.edges_end();++ei)
	{
		Point source=ei->source();
		Point des=ei->target();
		Segment s(source,des);
		EdgeMap[i++]=s;
	}
}


void DisplayMap()
{
	int listIDMap=tessellate1(mapP,0);
	int listIDVPolygon=tessellate1(visP,0);
	glColor3f(1,0,0);
	glCallList(listIDMap);
	glColor3f(0,1,0);
	glCallList(listIDVPolygon);
}



void DisplayLocalMap()
{
	glColor3f(0,1,0);
	int listIDVPolygon=tessellate1(visP,0);
	glCallList(listIDVPolygon);
}

void CalcHypothesis()
{
	if(hyps.size()==0)
	{
		g = HypothesisGenerator(mapP, visP, robotPos);
		hyps = g.GenHypothesis();
	}
}

void DisplayHypothesis()
{
	CalcHypothesis();
	int listIDMap=tessellate1(mapP,0);
	glColor3f(1,0,0);
	glCallList(listIDMap);
	glPointSize(10);
	glColor3f(0,0,1);
	glBegin(GL_POINTS);
    std::list<Point>::iterator it;
	for(it=hyps.begin();it!=hyps.end();++it)
	{
		glVertex2f((*it).cartesian(0),(*it).cartesian(1));
	}
	glEnd();

cout<<"Hypothesis done\n";
}

void CalcMajorityMap()
{
		CalcHypothesis();
		if(mmap.noOfHypothesis==0)
		{
			list<Point>::iterator it;
			int i=0;
			no_of_hypothesis=hyps.size();
			Point hypothesisArray[no_of_hypothesis];
			for(it = hyps.begin(); it != hyps.end(); it++)
			{
				hypothesisArray[i++]=*it;
			}

			Majoritymap mmap1(no_of_hypothesis,hypothesisArray,robotPos,mapP);
			mmap1.GenerateMajorityMap();
			mmap=mmap1;
			cout<<"Majority Map done\n";
		}
}

void DisplayMajorityMap()
{
	CalcMajorityMap();

	std::list<Faces>::iterator fit;

	for(fit=mmap.listMmapFaces.begin();fit!=mmap.listMmapFaces.end();++fit)
	{
		if((*fit).partOfMajorityMap)
		{
			glColor3f(0,1,0);

		}
		else
		{
			glColor3f(0,0,0);
			cout<<"Not majrity.....................................................................\n";
		}

		GLuint j=tessellate1((*fit).face,0);
		glCallList(j);
	}

/*
	std::list<Polygon>::iterator pit;
	for(pit=mmap.listTanslatedPolygons.begin();pit!=mmap.listTanslatedPolygons.end();++pit)
	{
		float rand1=(rand()%255);
		float rand2=(rand()%255);
		float rand3=(rand()%255);

		glColor3f(0,1,0);
		GLuint j=tessellate1((*pit),0);
		glCallList(j);
	}
*/

}

void CalcFaceContainingOrigin()
{
	CalcMajorityMap();
	if(polygonList.size()==0)
	{
		polygonList=mmap.findRegionContaningOrigin();
	}
}
void DisplayFaceContainingOrigin()
{
	CalcFaceContainingOrigin();
	glColor3f(0,1,0);
	std::list<Polygon>::iterator pit;
	for(pit=polygonList.begin();pit!=polygonList.end();++pit)
	{
		GLuint j=tessellate1(*pit,0);
		glCallList(j);
	}
}

Polygon CalcVisibilityPolygonEdge()
{
//	if(!VPEdge.size())
//	{
		Polygon VPEdge=pUtil.CalcWindowEdge(mapP,seg);
		return VPEdge;
		//	}
}
void DisplayVisibilityPolygonEdge()
{
		Polygon VPEdge=CalcVisibilityPolygonEdge();
		glColor3f(0,0,0);
		int listIDMap=tessellate1(mapP,0);
		glCallList(listIDMap);
		glColor3f(0,1,0);
		GLuint i=tessellate1(VPEdge,0);
		glCallList(i);

		glPointSize(10);
		glColor3f(0,0,1);
		glBegin(GL_LINES);
		Point s=seg.source();
		Point e=seg.end();
//		cout<<s<<"\n\n\n\n\n\n";
		glVertex2f(s.cartesian(0),s.cartesian(1));
		glVertex2f(e.cartesian(0),e.cartesian(1));
		glEnd();
}

void DisplayVisibilityPolygonPoint(Point point)
{

	glColor3f(0,0,0);
	int listIDMap=tessellate1(mapP,0);
	glCallList(listIDMap);
	if(pUtil.CheckInside(point,mapP))
	{
		Polygon p1=pUtil.CalcVisibilityPolygon(mapP,point);
//		pUtil.DisplayPolygon(p1);

		glColor3f(0,1,0);
		GLuint i=tessellate1(p1,0);
		glCallList(i);
	}
	glPointSize(5);
	glColor3f(0,0,1);
	glBegin(GL_POINTS);
	glVertex2f(point.cartesian(0),point.cartesian(1));
//	glVertex2f(10,10);
	glEnd();
//	glFlush();
}


void CalculateFijs()
{
	CalcFaceContainingOrigin();

if(Kijs.size()<=0 && Gijs.size()<=0 && Fijs.size()<=0 && all_two_faces.size()<=0)
{
	Point hypothesisArray[no_of_hypothesis];
	list<Point>::iterator it;
    int i=0;

    for(it = hyps.begin(); it != hyps.end(); it++)
	{
		hypothesisArray[i++]=*it;
	}

    //list<list<Polygon>> AllKij;


    list<Polygon> KPolygonList;  //For storing the Kij's
    for(i=0;i<no_of_hypothesis;i++)
    {
    	Point center=hypothesisArray[i];
    	list<Polygon> GPolygonList;  //for storing the gij's
    	list<Polygon> FPolygonList;  //for storing the gij's
//    	list<list<Polygon>> two_faces;
    	list<list<Polygon> > two_faces;
    	for(int j=0;j<no_of_hypothesis;j++)
    	{
    		Majoritymap overlayArrangement;
    		list<Polygon> TwoPolygons;
    		Vector t(robotPos.cartesian(0)-center.cartesian(0),robotPos.cartesian(1)-center.cartesian(1));
    		Transformation trans(CGAL::TRANSLATION,t);
    		TwoPolygons.push_back(overlayArrangement.GetTranslatePolygon(trans,mapP));
    		if(i!=j )
    		{
    			Vector t1(robotPos.cartesian(0)-hypothesisArray[j].cartesian(0),robotPos.cartesian(1)-hypothesisArray[j].cartesian(1));
        		Transformation trans1(CGAL::TRANSLATION,t1);
        		TwoPolygons.push_back(overlayArrangement.GetTranslatePolygon(trans1,mapP));
        		overlayArrangement.GenerateOverlay(TwoPolygons);
        		Polygon Fij=overlayArrangement.OverlayContaningOrigin(robotPos);

//        		cout<<"For Hypothesis "<<i<< "  " <<hypothesisArray[i]<<"  hypothesis  "<<j<<"  "<<hypothesisArray[j]<<endl;
//        		pUtil.DisplayPolygon(Fij);

        		Polygon Gij=pUtil.CalcGPolygon(robotPos,Fij,TwoPolygons);

//        		cout<<"Pushed\n\n";
        		two_faces.push_back(TwoPolygons);
        		FPolygonList.push_back(Fij);
        		GPolygonList.push_back(Gij);
    		}
    	}

    	cout<<"Fij , Gij Calculated for Hypothesis No:- "<<(i+1)<<"  Done\n";
    	Gijs.push_back(GPolygonList);
    	Fijs.push_back(FPolygonList);
    	all_two_faces.push_back(two_faces);
    	 // Using GPOLYGONLIST we need to construct Ki
    	 // Insert Ki in KPOLYGONLIST


//    	cout<<"Hypothesis  "<<hypothesisArray[i]<<endl;
    	list<Polygon>::iterator pit;
    	for(pit=GPolygonList.begin();pit!=GPolygonList.end();++pit)
    	{
//    		pUtil.DisplayPolygon(*pit);
    	}

    	Majoritymap Ki(GPolygonList.size(),GPolygonList);
    	Ki.GenerateOverlay(GPolygonList);
    	Ki.partMajority();

    	Kijs.push_back(Ki);
    	//Ki.PrintMajorityMap();

    	cout<<"Ki for hypothesis No:- "<<(i+1)<<"  Done\n\n";
    }
}
  /*

	cout<<"Length of all two faces is :- "<<all_two_faces.size()<<"   size of first list  "<<all_two_faces.front().size()<<"\n";
	cout<<"Length of all Fij is :- "<<Fijs.size()<<"\n";
	cout<<"Length of all Gij is :- "<<Gijs.size()<<"\n\n";
	cout<<"Length of all Kis is :- "<<Kijs.size()<<"\n\n";
*/

 /*
 * Displaying all the pair of hypothesis
 */
/*
    list<list<list<Polygon> > >::iterator pit3;
	list<list<Polygon> >::iterator pit2;
	list<Polygon> ::iterator pit1;
	int n=1;

	for(pit3=all_two_faces.begin();pit3!=all_two_faces.end();++pit3)
	{
		cout<<"For Hypothesis No:- "<<n++<<endl;
		for(pit2=(*pit3).begin();pit2!=(*pit3).end();++pit2)
		{
			cout<<"Size of list of two polygons :- "<<(*pit2).size()<<endl;
			cout<<"A pair of polygons\n";
			for(pit1=(*pit2).begin();pit1!=(*pit2).end();++pit1)
			{
				pUtil.DisplayPolygon(*pit1);
			}
		}
	}
*/


/*
	list<list<Polygon> >::iterator fit2=Fijs.begin();
	list<Polygon>::iterator fit1=(*fit2).begin();
	for(fit2=Fijs.begin();fit2!=Fijs.end();++fit2)
	{
		for(fit1=(*fit2).begin();fit1!=(*fit2).end();++fit1)
		{
			pUtil.DisplayPolygon(*fit1);
		}
	}

*/



}

void Display_polygon_as_line(Polygon P)
{
	EdgeIterator ei=P.edges_begin();
	glColor3f(0,0,0);
	glBegin(GL_LINE_LOOP);
	Point s=ei->source();
	glVertex3f(s.cartesian(0),s.cartesian(1),0);
	for(ei=P.edges_begin();ei!=P.edges_end();++ei)
	{
		Point d=ei->target();
		glVertex3f(d.cartesian(0),d.cartesian(1),0);
	}
	glEnd();
}

void Display_Fij()
{

if(!flagForKi)
{
	list<list<list<Polygon> > >::iterator pit3=all_two_faces.begin();
	list<list<Polygon> >::iterator pit2=(*pit3).begin();
	list<Polygon> ::iterator pit1;
	glColor3f(1,0,0);

		for(pit1=(*pit2).begin();pit1!=(*pit2).end();++pit1)
		{
			GLuint i=tessellate1((*pit1),0);
			glCallList(i);
			Display_polygon_as_line(*pit1);
			glColor3f(1,0,0);
		}
		cout<<"size of two faces  "<<all_two_faces.front().size()<<endl;

	glColor3f(0,1,0);
	list<list<Polygon> >::iterator fit2=Fijs.begin();
	list<Polygon>::iterator fit1=(*fit2).begin();
	GLuint i=tessellate1((*fit1),0);
	glCallList(i);


	glColor3f(0,0,1);
	list<list<Polygon> >::iterator git2=Gijs.begin();
	list<Polygon>::iterator git1=(*git2).begin();
	i=tessellate1((*git1),0);
	glCallList(i);
//	pUtil.DisplayPolygon(*git1);

	if(all_two_faces.front().size()==1 && Fijs.front().size()==1 && Gijs.front().size()==1)
	{
		all_two_faces.pop_front();
		Fijs.pop_front();
		Gijs.pop_front();
		flagForKi=true;

//		all_two_faces.front().pop_front();
	}
	else
	{
//		cout<<"size = 0\n\n";
		all_two_faces.front().pop_front();
		Fijs.front().pop_front();
		Gijs.front().pop_front();
		flagForKi=false;
	}
}
else
{
	glColor3f(0,0,1);
	list<Majoritymap>::iterator kit1=Kijs.begin();

	std::list<Faces>::iterator fit;
	for(fit=(*kit1).listMmapFaces.begin();fit!=(*kit1).listMmapFaces.end();++fit)
	{
		GLuint j=tessellate1((*fit).face,0);
		if((*fit).partOfMajorityMap)
		{
			glColor3f(0,1,0);
		}
		else
		{
			glColor3f(0,0,0);
		}
		glCallList(j);
	}
	Kijs.pop_front();
	flagForKi=false;

}

	cout<<"GIJ Drawn\n";
}


void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
/*
	gluLookAt(0, 0, -100,
	            0.0, 0.0,0.0 ,
	            0.0, 1.0, 0.0);
*/
	glLoadIdentity();
	gluPerspective(120/zoomfactor,1,0.1,16);
	gluLookAt(0,0,15,0,0,0,0,1,0);

	int edge_no;
	Point point;
	switch(phase)
	{
		case 0:
			LoadNewMap();
			break;
		case 1:
			DisplayMap();
			break;
		case 2:
			DisplayLocalMap();
			break;
		case 3:
			DisplayHypothesis();
			break;
		case 4:
			DisplayMajorityMap();
			break;
		case 5:
			DisplayFaceContainingOrigin();
			break;
		case 6:
			edge_no=edgeSpinner->get_int_val();
			cout<<"Edge No is :-  "<<edge_no<<"\n";
			seg=EdgeMap[edge_no];
			DisplayVisibilityPolygonEdge();
			break;
		case 7:
			point=Point(atof(shyp1.c_str()),atof(shyp2.c_str()));
			DisplayVisibilityPolygonPoint(point);
			break;
		case 8:
			CalculateFijs();
			break;
		case 10:
			Display_Fij();
			break;
		default:
			break;
	}
	glFlush();
}


void ShowMap(int ID)
{
	switch(ID)
	{
		case 0:
			phase=ID;
			glutPostRedisplay();
			break;
		case 1:
			phase=ID;  //ID=1
			glutPostRedisplay();
			break;
		case 2:
			phase=ID;   //ID=2
			glutPostRedisplay();
			break;
		case 3:
			phase=ID;  //ID=3
			glutPostRedisplay();
			break;
		case 4:
			phase=ID;	//ID=4
			glutPostRedisplay();
			break;
		case 5:
			phase=ID;	//ID=5
			glutPostRedisplay();
			break;
		case 6:
			phase=ID;	//ID=6
			glutPostRedisplay();
			break;
		case 7:
			phase=ID;	//ID=6
			glutPostRedisplay();
			break;
		case 8:
			FijPanel->hidden=false;
			phase=ID;
			glutPostRedisplay();
			break;
		case 10:
			phase=ID;
			glutPostRedisplay();
			break;
		default :
			break;
	}
}

string convert_point_string(Point p)
{

	GLUI_String s="(";
//	s.append(p.cartesian(0));
	s.append(",");
//	s.append(p.cartesian(1));
	s.append(")");
	return s;
}


int main(int argc, char ** argv)
{

	GLUI *glui;
	 GLUI_Panel *VisibilityPanel;
	  GLUI_Panel *HGererationPanel;
	  GLUI_EditText *hyp1,*hyp2,*Xcoordinate,*Ycoordinate;
      glutInit( &argc, argv );
	  glutInitDisplayMode( GLUT_RGB | GLUT_SINGLE );
	  glutInitWindowSize (WIDTH, HEIGHT);
	  glutInitWindowPosition (0, 0);
	  int windowId=glutCreateWindow ("Robot Localization");

	  init();
	  glutDisplayFunc( display );
//	  glutSpecialFunc(special);

	  GLUI_Master.set_glutIdleFunc (NULL);
	  /** Now create a GLUI user interface window and add controls **/

	  glui = GLUI_Master.create_glui( "GLUI", 0 ,840, 0);

	  glui->set_main_gfx_window( windowId );
	  mapList=glui->add_listbox("Choose Map  ",&mapID);
	  mapList->add_item(1,"Scenario1.txt");
	  mapMap[1]="Scenario1.txt";
	  mapList->add_item(2,"Scenario2.txt");
	  mapMap[2]="Scenario2.txt";
	  mapList->add_item(3,"Scenario3.txt");
	  mapMap[3]="Scenario3.txt";
	  mapList->add_item(4,"Office.txt");
	  mapMap[4]="Office.txt";
	  mapList->add_item(5,"Office_anti.txt");
	  mapMap[5]="Office_anti.txt";

	  glui->add_button("Choose Map",0,ShowMap);


	  GLUI_Spinner *zoomSpinner=glui->add_spinner("Perspective View",GLUI_SPINNER_FLOAT,&zoomfactor);
	  zoomSpinner->set_float_limits(0.1,1.5);
	  zoomSpinner->set_speed(0.4);

	  glui->add_button("Show Map",1,ShowMap);
	  glui->add_button("Show Visibility Map",2,ShowMap);
	  HGererationPanel=glui->add_panel("Hypothesis Generation");
	  glui->add_button_to_panel(HGererationPanel,"Hypothesis Set",3,ShowMap);
//	  HGererationPanel->hidden=true;
	  glui->add_button("Show Majority Map",4,ShowMap);
	  glui->add_button("Face Containing Origin",5,ShowMap);
	  glui->add_separator();

	  VisibilityPanel=glui->add_panel("Visibility Algorithms");

//	  edgeSpinner=glui->add_spinner_to_panel(VisibilityPanel,"Edge No.",GLUI_SPINNER_INT);
//	  edgeSpinner->set_int_limits(1,mapP.size(),GLUI_LIMIT_CLAMP);

//	  glui->add_button_to_panel(VisibilityPanel,"Visibility Polygon of edge",6,ShowMap);

	  glui->add_separator_to_panel(VisibilityPanel);

	  edgeSpinner=glui->add_spinner_to_panel(VisibilityPanel,"Edge No.",GLUI_SPINNER_INT);

	  glui->add_button_to_panel(VisibilityPanel,"Visibility Polygon of edge",6,ShowMap);

	  Xcoordinate=glui->add_edittext_to_panel(VisibilityPanel,"X",shyp1,GLUI_EDITTEXT_STRING);
	  Ycoordinate=glui->add_edittext_to_panel(VisibilityPanel,"Y",shyp2,GLUI_EDITTEXT_STRING);
	  glui->add_button_to_panel(VisibilityPanel,"Visibility Polygon of point",7,ShowMap);

	  glui->add_button("Calculate Fij's",8,ShowMap);
	  FijPanel=glui->add_panel("Calculating Fij's and Gij's");
	  hyp1=glui->add_edittext_to_panel(FijPanel,"Hypothesis 1",shyp1,GLUI_EDITTEXT_STRING);
	  hyp2=glui->add_edittext_to_panel(FijPanel,"Hypothesis 2",shyp2,GLUI_EDITTEXT_STRING);
	  glui->add_button_to_panel(FijPanel,"Start Computing",10,ShowMap);
	  FijPanel->hidden=true;

	  //	  hyp1->text=(string)Point(1,2);
//	  glui->add_statictext("COlor Represents this");
	  glutMainLoop();


      return 0;
}
