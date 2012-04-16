/*
 * GatingHierarchy.cpp
 *
 *  Created on: Mar 20, 2012
 *      Author: wjiang2
 */

#include "include/GatingHierarchy.hpp"
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/topological_sort.hpp>
#include <fstream>



/*need to be careful that gate within each node of the GatingHierarchy is
 * dynamically allocated,even it GatingHierarchy gets copied before destroyed
 * these gates are already gone since the gate objects were already freed by
 * this destructor
 */


GatingHierarchy::~GatingHierarchy()
{
	//for each node

	VertexID_vec vertices=getVertices(false);
	for (VertexID_vec::iterator it=vertices.begin();it!=vertices.end();it++)
	{

		cout<<"free the node properties from tree"<<endl;
		delete tree[*it];//free the property bundle
//			cout<<"free the indices"<<endl;
	}



//	if(data!=NULL)
//	{
//		cout<<"free the data object"<<endl;
//		delete data;
//	}



}

//default constructor without argument
GatingHierarchy::GatingHierarchy()
{
	dMode=1;
}
//constructor for sampleNode argument
//GatingHierarchy::GatingHierarchy(string sampleID,workspace * ws)
//{
//	thisWs=ws;
//
//	wsSampleNode curSampleNode=thisWs->getSample(sampleID);
//	wsRootNode root=thisWs->getRoot(curSampleNode);
//	VertexID pVerID=addRoot(thisWs->to_popNode(&root));
////	wsRootNode popNode=root;//getPopulation();
//	addPopulation(pVerID,&root);
//
//}
/*
 * Constructor that starts from a particular sampleNode from workspace to build a tree
 */
GatingHierarchy::GatingHierarchy(wsSampleNode curSampleNode,workspace * ws,bool isGating,ncdfFlow * _nc,unsigned short _dMode)
{
//	data=NULL;
	dMode=_dMode;
	thisWs=ws;
	nc=_nc;
	wsRootNode root=thisWs->getRoot(curSampleNode);
	VertexID pVerID=addRoot(thisWs->to_popNode(root));
//	wsRootNode popNode=root;//getPopulation();
	addPopulation(pVerID,&root,isGating);
	if(isGating)
		gating();

}
/*
 * add root node first before recursively add the other nodes
 * since root node does not have gates as the others do
 */
VertexID GatingHierarchy::addRoot(nodeProperties* rootNode)
{
	// Create  vertices in that graph
	VertexID u = boost::add_vertex(tree);

	tree[u]=rootNode;
//	nodelist[rootNode.getName()]=u;

	return(u);
}

/*
 * recursively append the populations to the tree
 */
void GatingHierarchy::addPopulation(VertexID parentID,wsNode * parentNode,bool isGating)
{


	wsPopNodeSet children =thisWs->getSubPop(parentNode);
	wsPopNodeSet::iterator it;
		for(it=children.begin();it!=children.end();it++)
		{
			//add boost node
			VertexID curChildID = boost::add_vertex(tree);
			wsPopNode curChildNode=(*it);
			//convert to the node format that GatingHierarchy understands
			nodeProperties *curChild=thisWs->to_popNode(curChildNode,isGating);
			if(dMode>=2)
				cout<<"node created:"<<curChild->getName()<<endl;
			//attach the populationNode to the boost node as property
			tree[curChildID]=curChild;
			//add relation between current node and parent node
			boost::add_edge(parentID,curChildID,tree);
			//update the node map for the easy query by pop name
//			nodelist[curChild.getName()]=curChildID;
			//recursively add its descendants
			addPopulation(curChildID,&curChildNode,isGating);
		}


}
/*
 * this is for semi-automated pipeline to add populations sequetially
 */
void GatingHierarchy::addGate(gate& g,string popName)
{

	typedef boost::graph_traits<populationTree>::vertex_descriptor vertex_t;

	// Create  vertices in that graph
//	vertex_t u = boost::add_vertex(tree);


//	vertex_t v = boost::add_vertex(g);

	// Create an edge conecting those two vertices
//	edge_t e; bool b;
//	boost::tie(e,b) = boost::add_edge(u,v,g);

//	boost::add_edge()
}

/*
 * subset operation is done within R,so there is no need for this member function
 * to apply subsetting within c++ thus avoid unnecessary numeric operation in c++
 */

flowData GatingHierarchy::getData(VertexID nodeID)
{
	cout<<"reading data from ncdf"<<endl;
	unsigned sampleInd=getSample().find_first_of(sampleName);

	flowData res=nc->readflowData(sampleInd);
	//subset the results by indices for non-root node
	if(nodeID>0)
	{
//		valarray<bool>* indices=vertexIDToNode(nodeID).thisIndice;
//		return res.subset(*indices);
		throw(domain_error("accessing data through non-root node is not supported yet!"));
	}
	else
		return res;
}
/*
 * load data from ncdfFlow file
 */
void GatingHierarchy::loadData()
{

	data=getData(0);
}

void GatingHierarchy::gating()
{
	cout <<"start gating..."<<endl;
	//read data once for all nodes
	if(data.data==NULL)
		loadData();
	VertexID_vec vertices=getVertices(true);

	for(VertexID_vec::iterator it=vertices.begin();it!=vertices.end();it++)
	{
		VertexID u=*it;
		nodeProperties * node=getNodeProperty(u);
		cout <<"gating on:"<<node->getName()<<endl;
		gate *g=node->getGate();
		g->gating(data,node->indices);
	}

	cout <<"finish gating..."<<endl;
}


/*
 * current output the graph in dot format
 * and further covert it to gxl in order for Rgraphviz to read since it does not support dot directly
 * right now the data exchange is through file system,it would be nice to do it in memory
 */
void GatingHierarchy::drawGraph(string output)
{
	ofstream outputFile(output.c_str());
	//...
	boost::write_graphviz(outputFile,tree,OurVertexPropertyWriterR(tree));
	outputFile.close();
//	system("dot2gxl ../output/test.dot -o ../output/test.gxl");
//	return("test.gxl");
//	system("pwd");


}

/*
 * retrieve the vertexIDs in topological order or in regular order
 */
VertexID_vec GatingHierarchy::getVertices(bool tsort=false){

	VertexID_vec res, vertices;
	if(tsort)
	{
		boost::topological_sort(tree,back_inserter(vertices));
		for(VertexID_vec::reverse_iterator it=vertices.rbegin();it!=vertices.rend();it++)
			res.push_back(*it);
	}
	else
	{
		VertexIt it_begin,it_end;
		tie(it_begin,it_end)=boost::vertices(tree);
		for(VertexIt it=it_begin;it!=it_end;it++)
			res.push_back((unsigned long)*it);
	}

	return(res);

}
/*
 * retrieve population names based on getVertices method
 * isPath flag indicates whether append the ancestor node names
 * the assumption is each node only has one parent
 */
vector<string> GatingHierarchy::getPopNames(bool tsort,bool isPath){

	VertexID_vec vertices=getVertices(tsort);
	vector<string> res;
	for(VertexID_vec::iterator it=vertices.begin();it!=vertices.end();it++)
	{
		VertexID u=*it;
		string nodeName=getNodeProperty(u)->getName();
		/*
		 * append ancestors on its way of tracing back to the root node
		 */
		if(isPath)
		{
			while(u>0)//if u==0, it is a root vertex
			{
				nodeName="/"+nodeName;
				VertexID_vec parents=getParent(u);
				if(parents.size()>1)
				{
					cout<<"multiple parent nodes."<<endl;
					break;
				}
				else
				{
					u=parents.at(0);
					if(u>0)//don't append the root node
						nodeName=getNodeProperty(u)->getName()+nodeName;
				}

			}


		}

		res.push_back(nodeName);

	}
	return res;
}
/*
 * using boost in_edges out_edges to retrieve adjacent vertices
 */
VertexID_vec GatingHierarchy::getParent(VertexID target){
	VertexID_vec res;
	if(target>=0&&target<=boost::num_vertices(tree)-1)
	{
//		cout<<"getting parent of "<<target<<"."<<tree[target].getName()<<endl;

		EdgeID e;
		boost::graph_traits<populationTree>::in_edge_iterator in_i, in_end;

		for (tie(in_i, in_end) = in_edges(target,tree);
			         in_i != in_end; ++in_i)
		{
		  e = *in_i;
		  VertexID  sarg = boost::source(e, tree);
		  res.push_back(sarg);
		}


	}
	else
	{
		cout<<"Warning:invalid vertexID:"<<target<<endl;
//		  res.push_back(0);

	}
	return(res);
}

VertexID_vec GatingHierarchy::getChildren(VertexID source){

	VertexID_vec res;
	if(source>=0&&source<=boost::num_vertices(tree)-1)
	{

		EdgeID e;
		boost::graph_traits<populationTree>::out_edge_iterator out_i, out_end;

		for (tie(out_i, out_end) = out_edges(source,tree);
				 out_i != out_end; ++out_i)
			{
			  e = *out_i;
			  VertexID  targ = target(e, tree);
			  res.push_back(targ);
			}
	}
	else
	{
		cout<<"invalid vertexID:"<<source<<endl;
//		res.push_back(0);
	}
	return(res);
}
/*
 * returning the reference of the vertex bundle
 */
nodeProperties * GatingHierarchy::getNodeProperty(VertexID u){


	if(u>=0&&u<=boost::num_vertices(tree)-1)
		return(tree[u]);
	else
	{
		throw(out_of_range("returning empty node due to the invalid vertexID:"+u));

	}
}
