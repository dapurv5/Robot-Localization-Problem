/*
 * Majoritymap.cpp
 *
 *  Created on: 22-Jan-2012
 *      Author: prateek
 */

#include "Majoritymap.h"

Majoritymap::Majoritymap() {
	// TODO Auto-generated constructor stub
 noOfHypothesis=0;
}

Majoritymap::Majoritymap(int n, Point H[],Point c,Polygon P)
{
	noOfHypothesis=n;
	center=c;
	map=P;
	hypothesis=H;
}

Majoritymap::Majoritymap(int n,std::list<Polygon> PolygonList )
{
	noOfHypothesis=n;
	listTanslatedPolygons=PolygonList;
}


Majoritymap::~Majoritymap() {
	// TODO Auto-generated destructor stub
}
/**
 * Prints the majority map
 */
void Majoritymap::PrintMajorityMap()
{
	list<Faces>::iterator f;
	int i;
	for(f=listMmapFaces.begin(),i=1;f!=listMmapFaces.end();++f,++i)
	{
		std::cout<<"Face No  "<<i<<"\n";
		f->PrintDescription();
	}
}

/**
 * @params polygonList: List Of Polygons
 * Generates the overlay arrangement of list of polygons
 */

void Majoritymap::GenerateOverlay(list<Polygon> polygonList)
{

	list<Polygon>::iterator pi;

	for(pi=polygonList.begin();pi!=polygonList.end();++pi)
	{
		for (EdgeIterator ei = pi->edges_begin(); ei != pi->edges_end(); ++ei)
		{
			Point s=ei->start();
			Point d=ei->end();
			Point_2 source(s.cartesian(0),s.cartesian(1));
			Point_2 destination(d.cartesian(0),d.cartesian(1));
			Segment_2 seg(source, destination);
			CGAL::insert (mmapArrangement,seg);
		}
	}


	Arrangement::Face_const_iterator fit;
//cout<<"In majority Map\n\n";
	for (fit = mmapArrangement.faces_begin(); fit != mmapArrangement.faces_end(); ++fit)
	{
		  if (!fit->is_unbounded())
		  {
			Polygon p=ConvertFaceToPolygon(fit->outer_ccb());
//			pUtil.DisplayPolygon(p);
			Faces f(p);
			listMmapFaces.push_back(f); //constructing the overlay Arrangement by adding the faces as polygons to it
		  }

	}
}

/**
 * Generates the majority map of overlay arrangement
 */
void Majoritymap::partMajority()
{

	listMmapFaces.clear();
	Arrangement::Face_const_iterator fit;

	for (fit = mmapArrangement.faces_begin(); fit != mmapArrangement.faces_end(); ++fit)
	{
		  if (!fit->is_unbounded())
		  {
			Polygon p=ConvertFaceToPolygon(fit->outer_ccb());
//			pUtil.DisplayPolygon(p);
			/*
			cout<<"Face Of Polygon \n";
			pUtil.DisplayPolygon(p);*/
			bool liesIn[noOfHypothesis];

			list<Polygon>::iterator pi;
			int i=0;
			int agreedByHypothesis=0;

			for(pi=listTanslatedPolygons.begin();pi!=listTanslatedPolygons.end();++pi)
			{
				if(IsContainedIn((*pi),p))  //return true if p lies in pi
				{
					liesIn[i++]=true;
					agreedByHypothesis++;
				}
				else
				{
					liesIn[i++]=false;
				}
			}
			bool isPartMmap=CheckPartOfMajorityMap(agreedByHypothesis,noOfHypothesis);

			Faces f(noOfHypothesis,p,liesIn,isPartMmap);
			listMmapFaces.push_back(f); //constructing the majority map by adding the faces as polygons to it
		  }
	}
}

/**
 * Generates the majority map
 */
void Majoritymap::GenerateMajorityMap()
{
	/*
	 * Generating the translated polygons according to the center
	 */
	for(int i=0;i<noOfHypothesis;i++)
	{
		Vector T((center.cartesian(0)-hypothesis[i].cartesian(0)),(center.cartesian(1)-hypothesis[i].cartesian(1)));
		Transformation translate(CGAL::TRANSLATION, T);
		Polygon p=GetTranslatePolygon(translate, map);
		/*cout<<"Translated Polygon \n";
		pUtil.DisplayPolygon(p);*/
		listTanslatedPolygons.push_back(p);
	}

		list<Polygon>::iterator pi;
		for(pi=listTanslatedPolygons.begin();pi!=listTanslatedPolygons.end();++pi)
		{
			for (EdgeIterator ei = pi->edges_begin(); ei != pi->edges_end(); ++ei)
			{
				Point s=ei->start();
				Point d=ei->end();
				Point_2 source(s.cartesian(0),s.cartesian(1));
				Point_2 destination(d.cartesian(0),d.cartesian(1));
				Segment_2 seg(source, destination);
				CGAL::insert (mmapArrangement,seg);
			}
		}

	/*
	 * now mmapArrangement contains all the faces required by us
	 */

		partMajority();
	}
/**
 * @params agree: No of hypothesis which agree
 * @params noOfHypothesis: total number of hypothesis
 * @returns bool if it is part of majority map
 */
bool Majoritymap::CheckPartOfMajorityMap(int agree, int noOfHypothesis)
{
	if(noOfHypothesis%2==0)
	{
		if(agree>= noOfHypothesis/2)
			return true;
	}
	else
	{
		if(agree> noOfHypothesis/2)
			return true;
	}
	return false;
}

/**
 * This function when given a face converts it into a polygon and returns the polygon
 *@params circ: Outer Boundary of face
 *@result Polygon corresponding to that face
 */
Polygon Majoritymap::ConvertFaceToPolygon(Arrangement::Ccb_halfedge_const_circulator circ)
{
	  Polygon poly;
	  Arrangement::Ccb_halfedge_const_circulator curr = circ;
	  do {
		  Point_2 p=(curr->source()->point());
		  Number_type x=p.cartesian(0);
		  Number_type y=p.cartesian(1);
		  poly.push_back(Point(to_double(x),to_double(y)));
	  } while (++curr != circ);
	  return poly;
}

/**
 * this function checks whether inner polygon is contained in outer polygon.
 * Note:- It only checks for the vertices of the inner polygon to be contained in the outer polygon as we don't have
 * the case when an edge will lie outside the outer polygon when the end points are in the polygon
 *@params outer:outer Polygon
 *@params inner: inner Polygon
 *@result bool if inner is present in outer
 */
Point midPoint(Point p1,Point p2)
{
	Point p((p1.cartesian(0)+p2.cartesian(0))/2,(p1.cartesian(1)+p2.cartesian(1))/2);
	return p;
}


/*
 * The function will only return a single a polygon as we have assumed that there are no holes in
 * the majorty rule map.
 */
Polygon Majoritymap::generateUnionFaces()
{
	list<Polygon> unionpoly;
	list<Polygon> faces;
	list<Faces>::iterator fit;
	for(fit=listMmapFaces.begin();fit!=listMmapFaces.end();++fit)
	{
		if(fit->partOfMajorityMap)
		{
			Polygon p=fit->face;
			faces.push_back(p);
		}
	}
	unionpoly=pUtil.unionPolygons(faces);
	if(unionpoly.size()==1)
		return unionpoly.front();
	return unionpoly.front();
}




bool CheckInside(Point& pt,Polygon& polygon)
{
	bool flag = false;
	Point *pgn_begin = polygon.vertices_begin().base();
	Point *pgn_end = polygon.vertices_end().base();

	switch(CGAL::bounded_side_2(pgn_begin, pgn_end,pt, K())) {
	case CGAL::ON_BOUNDED_SIDE :
		flag = true;
		break;
	}
	return flag;
}

bool Majoritymap::IsContainedIn(Polygon outer,Polygon inner)
{
/*
	for(EdgeIterator ei = inner.edges_begin(); ei !=inner.edges_end(); ++ei)
	{

		Point first=(*ei).point(0);
		Point second=(*ei).point(1);
		if(!pUtil.IsInsidePolygon1(first,second,outer))
		{
			cout<<"First Point"<<first<<" Second Point "<<second<<"\n";
			cout<<"Inner Polygon \n";
			pUtil.DisplayPolygon(inner);
			cout<<"Outer Polygon \n";
			pUtil.DisplayPolygon(outer);
			return false;
		}
	}

	return true;
*/

	inner=pUtil.RemoveCollinearPoints(inner);
	EdgeIterator ei=inner.edges_begin();
	Segment seg=*(ei);
	Point firstMid=midPoint(seg.source(),seg.target());
	++ei;
	for(;ei!=inner.edges_end();++ei)
	{
		Point first=(*ei).point(0);
		Point second=(*ei).point(1);
		Point midPoint1=midPoint(first,second);
		Point check=midPoint(firstMid,midPoint1);
		if(CheckInside(check,inner))
		{
			if(CheckInside(check,outer))
			{
				return true;
			}
			else
				return false;
		}
	}
	return false;
}

/**
 * This function applies the transformation on a polygon and returns a new polygon
 *@params translate: Transformation object
 *@returns the translated polygon
 */
Polygon Majoritymap::GetTranslatePolygon(Transformation& translate, Polygon& polygon){

	Polygon translatedPolygon;

	for(VertexIterator vi = polygon.vertices_begin(); vi !=polygon.vertices_end(); ++vi){
		translatedPolygon.push_back(translate(*vi));
	}
	return translatedPolygon;
}
/**
 * @params center: Point
 * @returns polygon containing the center
 */

Polygon Majoritymap::OverlayContaningOrigin(Point &center){

	Polygon p;
	list<Faces>::iterator f;

	for(f=listMmapFaces.begin(); f!=listMmapFaces.end(); ++f)
	{
		if(pUtil.CheckInside(center,f->face))
				{
					return (f->face);
				}

	}
	return p;
}
/**
 * @returns list of polygons for region containing origin
 */
list<Polygon> Majoritymap::findRegionContaningOrigin(){

	list<Polygon> traversablePolygons;
	list<Faces>::iterator f;

	for(f=listMmapFaces.begin(); f!=listMmapFaces.end(); ++f)
	{
		if(f->partOfMajorityMap){
			traversablePolygons.push_back(f->face);
		}
	}

	int i = 0;
	list<Polygon>::iterator it;

	Polygon traverablePolygonArr[traversablePolygons.size()];
	for(it = traversablePolygons.begin(); it != traversablePolygons.end(); ++it){
		traverablePolygonArr[i++] = *it;
	}

	int centerIndex = -1;

	for(i = 0; i < traversablePolygons.size(); i++){
		if(pUtil.CheckInside(center, traverablePolygonArr[i])){
			centerIndex = i;
			break;
		}
	}

	using namespace boost;
	typedef adjacency_list < vecS, vecS, undirectedS > Graph;
	typedef graph_traits < Graph >::vertex_descriptor Vertex;
	const int N = traversablePolygons.size();
	Graph G(N);

	for(i = 0; i < traversablePolygons.size(); i++){
		for(int j = i+1; j < traversablePolygons.size(); j++){
			if(areAdjacent(traverablePolygonArr[i], traverablePolygonArr[j])){
				add_edge(i,j,G);
			}
		}
	}

	 std::vector<int> component(num_vertices(G));
	 int num = connected_components
			 (G,
			  make_iterator_property_map(component.begin(),
			  get(vertex_index, G),
			  component[0]));

	 int centersComponent = component[centerIndex];
	 list<Polygon> originRegion;
	 for(i=0; i < traversablePolygons.size(); i++){
		 if(component[i] == centersComponent){
			 originRegion.push_back(traverablePolygonArr[i]);
		 }
	 }

	 return originRegion;
}

/**
 * @params poly1,poly2
 * @returns if poly1 and poly2 are adjacent polygons
 */
bool Majoritymap::areAdjacent(Polygon& poly1, Polygon& poly2){

	for (EdgeIterator ei = poly1.edges_begin(); ei != poly1.edges_end(); ++ei)
	{
		for (EdgeIterator f=poly2.edges_begin();f!=poly2.edges_end();++f)
		{
			if (CGAL::do_intersect(*ei,*f))
				return true;
		}
	}
	return false;
}

