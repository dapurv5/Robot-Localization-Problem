/*
 * HypothesisGenerator.cpp
 *
 *  Created on: 14-Jan-2012
 *      Author: apurv
 */

#include "HypothesisGenerator.h"
using namespace boost::numeric::ublas;


HypothesisGenerator::HypothesisGenerator(Polygon& mapP, Polygon& visP, Point& robotPos) {
	this->mapP = mapP;
	this->visP = visP;
	this->pUtil= PolygonUtil();
	this->robotPos = robotPos;
}

HypothesisGenerator::HypothesisGenerator(Polygon& mapP, Polygon& visP,Point& robotPos, PolygonUtil& pUtil){
	this->mapP = mapP;
	this->visP = visP;
	this->pUtil= pUtil;
	this->robotPos = robotPos;
}

HypothesisGenerator::~HypothesisGenerator() {

}

Segment HypothesisGenerator::GetNextEdge(EdgeIterator& ei, Polygon& polygon){
	if (ei == (polygon.edges_end()-1)){
		ei = polygon.edges_begin();
	}
	else{
		ei++;
	}
	return *ei;
}


list<Point> HypothesisGenerator::GenHypothesis(){
	list<Point> hyps;
	EdgeIterator mapIter = mapP.edges_begin();
	EdgeIterator visIter = visP.edges_begin();

	EdgeIterator mapIterCpy = mapP.edges_begin();
	EdgeIterator visIterCpy = visP.edges_begin();

	Segment mapEdge = *(mapIter);
	Segment visEdge = *(visIter);


	for(int i = 0; i < mapP.size(); i++){

		visIter = visP.edges_begin();
		visIterCpy = visP.edges_begin();

		for(int j = 0; j < visP.size(); j++){

			if(IsMatch(mapEdge, visEdge)){

				Vector T(visEdge.point(0), mapEdge.point(0));
				Vector invT(mapEdge.point(0), visEdge.point(0));

				Transformation translate(CGAL::TRANSLATION, T);
				Transformation invTranslate(CGAL::TRANSLATION, invT);

				TranslatePolygon(translate, visP);
				robotPos = translate(robotPos);


				if(IsComplMatch( visIterCpy)){

					Point newHyp(robotPos.cartesian(0), robotPos.cartesian(1));
					translate(newHyp);
					if(!IsInList(hyps,newHyp)){
						hyps.push_back(newHyp);
					}

					mapIterCpy = mapIter;
					visIterCpy = visIter;

				}

				TranslatePolygon(invTranslate, visP);
				robotPos = invTranslate(robotPos);
			}
			visEdge = GetNextEdge(visIter, visP);
			GetNextEdge(visIterCpy, visP);
		}
		mapEdge = GetNextEdge(mapIter, mapP);
		GetNextEdge(mapIterCpy, mapP);
	}
	return hyps;
}


bool HypothesisGenerator::IsInList(list<Point> hyps, Point p){
	list<Point>::iterator it;
	for(it = hyps.begin(); it != hyps.end(); it++){
		if(p.cartesian(0) == (*it).cartesian(0)  && p.cartesian(1) == (*it).cartesian(1) ){
			return true;
		}
	}
	return false;
}


void HypothesisGenerator::TranslatePolygon(Transformation& translate, Polygon& polygon){

	for(VertexIterator vi = polygon.vertices_begin(); vi !=polygon.vertices_end(); ++vi){
		*vi = translate(*vi);
	}
}


bool HypothesisGenerator::IsComplMatch( EdgeIterator& visIter){

	Segment visEdge = *(visIter);
	int i = 0;

	while(i < visP.size()){

		Point p1 = visEdge.point(0);
		Point p2 = visEdge.point(1);

		if( !(mapP.has_on_boundary(p1) && mapP.has_on_boundary(p2)) ){
			return false;
		}

		if( !TwoVertexOnEdge(visEdge)){
			return false;
		}

		i++;
		visEdge = GetNextEdge(visIter, visP);
	}

	return true;
}



/**
 * Returns whether edge has atmost two vertices of polygon lying on this edge.
 * @param edge
 * @return
 */
bool HypothesisGenerator::TwoVertexOnEdge(Segment& edge){

	int count = 0;
	for(VertexIterator vi = mapP.vertices_begin(); vi !=mapP.vertices_end(); ++vi){
		Point p = *vi;
		if(edge.has_on(p)){count++;}
	}

	return (count <= 2);
}




bool HypothesisGenerator::IsMatch(Segment mapEdge, Segment visEdge){
	return (length(mapEdge) == length(visEdge) && orient(mapEdge) == orient(mapEdge) );
}



double HypothesisGenerator::GetSlope(Segment& edge){
	Point p2 = edge.point(1);
	Point p1 = edge.point(0);
	if(edge.is_vertical())
		return INFINITY;
	else
		return ( p2.cartesian(1) - p1.cartesian(1))/(p2.cartesian(0) - p1.cartesian(0));
}




/**
 * Returns the square of length of an edge.
 */
double HypothesisGenerator::length(Segment edge){
	return edge.squared_length();
}

double HypothesisGenerator::orient(Segment edge){
	return GetSlope(edge);
}


bool cmp_segments::operator()(Segment s1, Segment s2){
	Point p11 = s1.point(0);
	Point p12 = s1.point(1);

	Point p21 = s2.point(0);
	Point p22 = s2.point(1);

	return ( (p11.cartesian(0) == p21.cartesian(0)) && (p11.cartesian(1) == p21.cartesian(1)));
}