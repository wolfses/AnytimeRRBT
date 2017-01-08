// This file is part of the URDF PLUGIN for V-REP
//  
// Copyright 2006-2014 Coppelia Robotics GmbH. All rights reserved. 
// marc@coppeliarobotics.com
// www.coppeliarobotics.com
// 
// A big thanks to Ignacio Tartavull for his precious help!
// 
// The URDF PLUGIN is licensed under the terms of GNU GPL:
// 
// -------------------------------------------------------------------
// The URDF PLUGIN is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// THE URDF PLUGIN IS DISTRIBUTED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
// WARRANTY. THE USER WILL USE IT AT HIS/HER OWN RISK. THE ORIGINAL
// AUTHORS AND COPPELIA ROBOTICS GMBH WILL NOT BE LIABLE FOR DATA LOSS,
// DAMAGES, LOSS OF PROFITS OR ANY OTHER KIND OF LOSS WHILE USING OR
// MISUSING THIS SOFTWARE.
// 
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with the URDF PLUGIN.  If not, see <http://www.gnu.org/licenses/>.
// -------------------------------------------------------------------

//
// This file was automatically created for V-REP release V3.2.0 on Feb. 3rd 2015

// The URDF plugin is courtesy of Ignacio Tartavull. A few modifications by Marc.

#include "link.h"

urdfLink::urdfLink()
{
	//Initialize arrays
		//Variables Inertial
		 inertial_xyz[0]= 0;			inertial_xyz[1]= 0;				inertial_xyz[2]= 0;
		 inertial_rpy[0]= 0;			inertial_rpy[1]= 0;				inertial_rpy[2]= 0;
		
		//Variables Visual				
		 visual_xyz[0]= 0;				visual_xyz[1]= 0;				visual_xyz[2]= 0;
		 visual_rpy[0]= 0;				visual_rpy[1]= 0;				visual_rpy[2]= 0;
		 visual_box_size[0]= 0;			visual_box_size[1]= 0;			visual_box_size[2]= 0;	
		 visual_sphere_size[0]= 0;		visual_sphere_size[1]= 0;		visual_sphere_size[2]= 0;
		 visual_cylinder_size[0]= 0;    visual_cylinder_size[1]= 0;		visual_cylinder_size[2]= 0;
		 visual_rgba[0]=0.4f;			visual_rgba[1]=0.4f;			visual_rgba[2]=0.4f;	visual_rgba[3]=0.0f;
		 visual_mesh_scaling[0]=1.0f;
		 visual_mesh_scaling[1]=1.0f;
		 visual_mesh_scaling[2]=1.0f;

		//Variables Collision
		 collision_xyz[0]= 0;			 collision_xyz[1]= 0;			collision_xyz[2]= 0;
		 collision_rpy[0]= 0;			 collision_rpy[1]= 0;		    collision_rpy[2]= 0;	
		 collision_box_size[0]= 0;		 collision_box_size[1]= 0;	    collision_box_size[2]= 0;	
		 collision_sphere_size[0]= 0;	 collision_sphere_size[1]= 0;	collision_sphere_size[2]= 0;
		 collision_cylinder_size[0]= 0;	 collision_cylinder_size[1]= 0;	collision_cylinder_size[2]= 0;
		 collision_mesh_scaling[0]=1.0f;
		 collision_mesh_scaling[1]=1.0f;
		 collision_mesh_scaling[2]=1.0f;

		 mass = 1.0f;
		 inertia[8] = 0.0f; inertia[7] = 0; inertia[6] = 0; inertia[5] = 0; inertia[4] = 0.0f; inertia[3] = 0; inertia[2] = 0; inertia[1] = 0; inertia[0] = 0.0f;
		inertiaPresent=false;

		nParent=-1;
		nChild=-1;
		nLinkVisual=-1;
		nLinkCollision=-1;
}

urdfLink::~urdfLink()
{
}

void urdfLink::setPosition(std::string gazebo_xyz,std::string choose)
{
	if(choose == "inertial")
		stringToArray(inertial_xyz,gazebo_xyz.c_str());
	if(choose == "visual")
		stringToArray(visual_xyz,gazebo_xyz.c_str());
	if(choose == "collision")
		stringToArray(collision_xyz,gazebo_xyz.c_str());
}

void urdfLink::setRotation(std::string gazebo_rpy,std::string choose)
{
	if(choose == "inertial")
		stringToArray(inertial_rpy,gazebo_rpy.c_str());
	if(choose == "visual")
		stringToArray(visual_rpy,gazebo_rpy.c_str());
	if(choose == "collision")
		stringToArray(collision_rpy,gazebo_rpy.c_str());
}


void urdfLink::setBox(std::string gazebo_size,std::string choose)
{
	if(choose == "visual")
	{
		stringToArray(visual_box_size, gazebo_size);
	}
	if(choose == "collision")
	{
		stringToArray(collision_box_size, gazebo_size);
	}
}
void urdfLink::setSphere(std::string gazebo_radius,std::string choose)
{
	if(choose == "visual")
	{
		stringToArray(visual_sphere_size,gazebo_radius+" "+gazebo_radius+" "+gazebo_radius);

		visual_sphere_size[0] = visual_sphere_size[0] * 2; //Radius to bounding box conversion
		visual_sphere_size[1] = visual_sphere_size[1] * 2; //Radius to bounding box conversion
		visual_sphere_size[2] = visual_sphere_size[2] * 2; //Radius to bounding box conversion

	}
	if(choose == "collision")
	{
		stringToArray(collision_sphere_size,gazebo_radius+" "+gazebo_radius+" "+gazebo_radius);
		
		collision_sphere_size[0] = collision_sphere_size[0] * 2; //Radius to bounding box conversion
		collision_sphere_size[1] = collision_sphere_size[1] * 2; //Radius to bounding box conversion
		collision_sphere_size[2] = collision_sphere_size[2] * 2; //Radius to bounding box conversion
	}
}
void urdfLink::setCylinder(std::string gazebo_radius,std::string gazebo_length,std::string choose)
{
	if(choose == "visual")
	{
		stringToArray(visual_cylinder_size,gazebo_radius+" "+gazebo_radius+" "+gazebo_length);

		visual_cylinder_size[0] = visual_cylinder_size[0] * 2; //Radius to bounding box conversion
		visual_cylinder_size[1] = visual_cylinder_size[1] * 2; //Radius to bounding box conversion
	
	}
	if(choose == "collision")
	{
		stringToArray(collision_cylinder_size,gazebo_radius+" "+gazebo_radius+" "+gazebo_length);

		collision_cylinder_size[0] = collision_cylinder_size[0] * 2; //Radius to bounding box conversion
		collision_cylinder_size[1] = collision_cylinder_size[1] * 2; //Radius to bounding box conversion
		
	}
}

void urdfLink::setColor(std::string color)
{
	stringToArray(visual_rgba,color);
}

void urdfLink::setMass(std::string gazebo_mass)
{
	mass=getFloat(gazebo_mass);
}
void urdfLink::setInertia(int position, std::string gazebo_inertia_number)
{
	inertia[position] = getFloat(gazebo_inertia_number);
}
void urdfLink::verifyInertia()
{
	float c=0.0f;
	for (int i=0;i<9;i++)
		c+=fabs(inertia[i]);
	if (c==0.0f)
	{
		std::string txt("ERROR: found an invalid inertia entry for link '"+ name+"'");
		printToConsole(txt.c_str());

		inertia[0]=0.001f;
		inertia[4]=0.001f;
		inertia[8]=0.001f;
	}
}

void urdfLink::setMeshFilename(std::string packagePath,std::string meshFilename,std::string choose)
{
	std::string meshFilename_alt; // we use an alternative filename... the package location is somewhat strangely defined sometimes!!
	if (meshFilename.compare(0,10,"package://")==0) // condition added by Marc on 17/1/2014
	{
		meshFilename = meshFilename.substr(9,meshFilename.size()); //to delete de package:/ part
		meshFilename_alt=meshFilename;
		meshFilename = packagePath + meshFilename;
		packagePath = packagePath.substr(0, packagePath.find_last_of("/"));
		meshFilename_alt = packagePath + meshFilename_alt;
	}

	std::string extension = meshFilename.substr(meshFilename.size()-3,meshFilename.size());
	int nExtension;
	if(extension == "obj" || extension =="OBJ"){ nExtension = 0;}
	else if(extension == "dxf" || extension == "DXF"){ nExtension = 1;}
	else if(extension == "3ds" || extension == "3DS"){ nExtension = 2;}
	else if(extension == "stl" || extension == "STL"){ nExtension = 4;}
	else if(extension == "dae" || extension == "DAE"){ nExtension = 5;}
	else
	{
		nExtension = -1;
		std::string txt("ERROR: the extension '"+ extension +"' is not currently a supported.");
		printToConsole(txt.c_str());
	}

	if(choose == "visual")
	{
		visual_meshFilename = meshFilename;
		visual_meshFilename_alt = meshFilename_alt;
		visual_meshExtension = nExtension;
	}
	if(choose == "collision")
	{
		collision_meshFilename = meshFilename;		
		collision_meshFilename_alt = meshFilename_alt;
		collision_meshExtension = nExtension;
	}
}

//Write
void urdfLink::createLink(bool hideCollisionLinks,bool convexDecomposeNonConvexCollidables,bool createVisualIfNone,bool& showConvexDecompositionDlg)
{
	std::string txt("Creating link '"+name+"'...");
	printToConsole(txt.c_str());

	//visual
	if(!visual_meshFilename.empty())
	{
		std::string fname(visual_meshFilename);
		bool exists=true;
		bool useAlt=false;
		if (!simDoesFileExist(fname.c_str()))
		{
			fname=visual_meshFilename_alt;
			exists=simDoesFileExist(fname.c_str());
			useAlt=true;
		}

		if (!exists)
			printToConsole("ERROR: the mesh file could not be found.");
		else
			nLinkVisual = simImportShape(visual_meshExtension,fname.c_str(),0,0.0001f,1.0);

		if (nLinkVisual == -1)
		{
			if (!useAlt)
				txt="ERROR: failed to create the mesh '"+visual_meshFilename+"' with extension type "+boost::lexical_cast<std::string>(visual_meshExtension);
			else
				txt="ERROR: failed to create the mesh '"+visual_meshFilename+"' or '"+visual_meshFilename_alt+"' with extension type "+boost::lexical_cast<std::string>(visual_meshExtension);
			printToConsole(txt.c_str());
		}
		else
			nLinkVisual=scaleShapeIfRequired(nLinkVisual,visual_mesh_scaling);
	}
	else if (!isArrayEmpty(visual_sphere_size))
		nLinkVisual = simCreatePureShape( 1,1+2+16, visual_sphere_size, mass, NULL);
	else if (!isArrayEmpty(visual_cylinder_size))
		nLinkVisual = simCreatePureShape( 2,1+2+16, visual_cylinder_size, mass, NULL);
	else if (!isArrayEmpty(visual_box_size))
		nLinkVisual = simCreatePureShape( 0,1+2+16, visual_box_size, mass, NULL);


	//collision
	if(!collision_meshFilename.empty())
	{ 	
		std::string fname(collision_meshFilename);
		bool exists=true;
		bool useAlt=false;
		if (!simDoesFileExist(fname.c_str()))
		{
			fname=collision_meshFilename_alt;
			exists=simDoesFileExist(fname.c_str());
			useAlt=true;
		}

		if (!exists)
			printToConsole("ERROR: the mesh file could not be found");
		else
			nLinkCollision = simImportShape(collision_meshExtension,fname.c_str(),0,0.0001f,1.0);

		if (nLinkCollision == -1)
		{
			if (!useAlt)
				txt="ERROR: failed to create the mesh '"+collision_meshFilename+"' with extension type "+boost::lexical_cast<std::string>(collision_meshExtension);
			else
				txt="ERROR: failed to create the mesh '"+collision_meshFilename+"' or '"+collision_meshFilename_alt+"' with extension type "+boost::lexical_cast<std::string>(collision_meshExtension);
			printToConsole(txt.c_str());
		}
		else
		{
			nLinkCollision=scaleShapeIfRequired(nLinkCollision,collision_mesh_scaling);
			if (createVisualIfNone&&(nLinkVisual==-1))
			{ // We create a visual from the collision shape (before it gets morphed hereafter):
				simRemoveObjectFromSelection(sim_handle_all,-1);
				simAddObjectToSelection(sim_handle_single,nLinkCollision);
				simCopyPasteSelectedObjects();
				nLinkVisual=simGetObjectLastSelection();
			}
			int p;
			int convInts[5]={1,500,200,0,0}; // 3rd value from 100 to 500 on 5/2/2014
			float convFloats[5]={100.0f,30.0f,0.25f,0.0f,0.0f};
			if ( convexDecomposeNonConvexCollidables&&(simGetObjectIntParameter(nLinkCollision,3017,&p)>0)&&(p==0) )
			{
				int aux=1+4+8+16+64;
				if (showConvexDecompositionDlg)
					aux=1+2+8+16+64;
				showConvexDecompositionDlg=false;
				simConvexDecompose(nLinkCollision,aux,convInts,convFloats); // we generate convex shapes!
			}
			simSetObjectIntParameter(nLinkCollision,3003,!inertiaPresent); // we make it non-static if there is an inertia
			simSetObjectIntParameter(nLinkCollision,3004,1); // we make it respondable since it is a collision object
		}

	}
	else if (!isArrayEmpty(collision_sphere_size))
		nLinkCollision = simCreatePureShape( 1,1+2+4+8+16*(!inertiaPresent), collision_sphere_size, mass, NULL);
	else if (!isArrayEmpty(collision_cylinder_size))
		nLinkCollision = simCreatePureShape( 2,1+2+4+8+16*(!inertiaPresent), collision_cylinder_size, mass, NULL);
	else if (!isArrayEmpty(collision_box_size))
		nLinkCollision = simCreatePureShape( 0,1+2+4+8+16*(!inertiaPresent), collision_box_size, mass, NULL);

	// Inertia
	if (inertiaPresent)
	{
		if (nLinkCollision==-1)
		{ // we do not have a collision object. Let's create a dummy collision object, since inertias can't exist on their own in V-REP:
			float dummySize[3]={0.05f,0.05f,0.05f};
			nLinkCollision = simCreatePureShape( 1,1+2+4, dummySize, mass, NULL); // we make it non-respondable!
		}


		C7Vector inertiaFrame;
		inertiaFrame.X.set(inertial_xyz);
		inertiaFrame.Q=getQuaternionFromRpy(inertial_rpy);

		C7Vector collisionFrame;
		collisionFrame.X.set(collision_xyz);
		collisionFrame.Q=getQuaternionFromRpy(collision_rpy);

		C4X4Matrix x((collisionFrame.getInverse()*inertiaFrame).getMatrix());
		float i[12]={x.M(0,0),x.M(0,1),x.M(0,2),x.X(0),x.M(1,0),x.M(1,1),x.M(1,2),x.X(1),x.M(2,0),x.M(2,1),x.M(2,2),x.X(2)};
		simSetShapeMassAndInertia(nLinkCollision,mass,inertia,C3Vector::zeroVector.data,i);
		//std::cout << "Mass: " << mass << std::endl;
	}
	else
	{
		if (nLinkCollision!=-1)
		{
			std::string txt("ERROR: found a collision object without inertia data for link '"+ name+"'. Is that link meant to be static?");
			printToConsole(txt.c_str());
		}
	}

	if (createVisualIfNone&&(nLinkVisual==-1)&&(nLinkCollision!=-1))
	{ // We create a visual from the collision shape (meshes were handled earlier):
		simRemoveObjectFromSelection(sim_handle_all,-1);
		simAddObjectToSelection(sim_handle_single,nLinkCollision);
		simCopyPasteSelectedObjects();
		nLinkVisual=simGetObjectLastSelection();
		simSetObjectIntParameter(nLinkVisual,3003,1); // we make it static since only visual
		simSetObjectIntParameter(nLinkVisual,3004,0); // we make it non-respondable since only visual
	}

	// Set the respondable mask:
	if (nLinkCollision!=-1)
		simSetObjectIntParameter(nLinkCollision,3019,0xff00); // colliding with everything except with other objects in that tree hierarchy

	// Set the names, visibility, etc.:
	if (nLinkVisual!=-1)
	{
		setVrepObjectName(nLinkVisual,std::string(name+"_visual").c_str());
		const float specularDiffuse[3]={0.3f,0.3f,0.3f};
		simSetShapeColor(nLinkVisual,NULL,0,visual_rgba);
		simSetShapeColor(nLinkVisual,NULL,1,specularDiffuse);
		simSetShapeColor(nLinkVisual,NULL,2,specularDiffuse);
		if (nLinkCollision!=-1)
		{ // if we have a collision object, we attach the visual object to it, then forget the visual object
			C7Vector visualFrame;
			visualFrame.X.set(visual_xyz);
			visualFrame.Q=getQuaternionFromRpy(visual_rpy);

			C7Vector collisionFrame;
			collisionFrame.X.set(collision_xyz);
			collisionFrame.Q=getQuaternionFromRpy(collision_rpy);

			C7Vector initVisualFrame;
			simGetObjectPosition(nLinkVisual,-1,initVisualFrame.X.data);
			C3Vector euler;
			simGetObjectOrientation(nLinkVisual,-1,euler.data);
			initVisualFrame.Q.setEulerAngles(euler);

			C7Vector initCollisionFrame;
			simGetObjectPosition(nLinkCollision,-1,initCollisionFrame.X.data);
			simGetObjectOrientation(nLinkCollision,-1,euler.data);
			initCollisionFrame.Q.setEulerAngles(euler);

			C7Vector x(initCollisionFrame.getInverse()*collisionFrame.getInverse()*visualFrame*initVisualFrame);

			simSetObjectPosition(nLinkVisual,-1,x.X.data);
			simSetObjectOrientation(nLinkVisual,-1,x.Q.getEulerAngles().data);
			simSetObjectParent(nLinkVisual,nLinkCollision,0);
		}
	}
	if (nLinkCollision!=-1)
	{
		setVrepObjectName(nLinkCollision,std::string(name+"_respondable").c_str());
		if (hideCollisionLinks)
			simSetObjectIntParameter(nLinkCollision,10,256); // we "hide" that object in layer 9
	}

}

int urdfLink::scaleShapeIfRequired(int shapeHandle,float scalingFactors[3])
{ // in future there will be a non-iso scaling function for objects in V-REP, but until then...
	if ( (scalingFactors[0]*scalingFactors[1]*scalingFactors[2]>0.99999f)&&(scalingFactors[0]>0.0f)&&(scalingFactors[1]>0.0f) )
		return(shapeHandle); // no scaling required!
	if (fabs(scalingFactors[0])<0.00001f)
		scalingFactors[0]=0.00001f*scalingFactors[0]/fabs(scalingFactors[0]);
	if (fabs(scalingFactors[1])<0.00001f)
		scalingFactors[1]=0.00001f*scalingFactors[1]/fabs(scalingFactors[1]);
	if (fabs(scalingFactors[2])<0.00001f)
		scalingFactors[2]=0.00001f*scalingFactors[2]/fabs(scalingFactors[2]);
	int newShapeHandle=shapeHandle;
	float* vertices;
	int verticesSize;
	int* indices;
	int indicesSize;
	if (simGetShapeMesh(shapeHandle,&vertices,&verticesSize,&indices,&indicesSize,NULL)!=-1)
	{
		// Scale the vertices:
		C7Vector tr;
		simGetObjectPosition(shapeHandle,-1,tr.X.data);
		C3Vector euler;
		simGetObjectOrientation(shapeHandle,-1,euler.data);
		tr.Q.setEulerAngles(euler);
		for (int i=0;i<verticesSize/3;i++)
		{
			C3Vector v(vertices+3*i);
			v*=tr;
			v(0)*=scalingFactors[0];
			v(1)*=scalingFactors[1];
			v(2)*=scalingFactors[2];
			vertices[3*i+0]=v(0);
			vertices[3*i+1]=v(1);
			vertices[3*i+2]=v(2);
		}
		// Flip the triangles (if needed)
		if (scalingFactors[0]*scalingFactors[1]*scalingFactors[2]<0.0f)
		{
			for (int i=0;i<indicesSize/3;i++)
			{
				int tmp=indices[3*i+0];
				indices[3*i+0]=indices[3*i+1];
				indices[3*i+1]=tmp;
			}
		}
		// Remove the old shape and create a new one with the scaled data:
		simRemoveObject(shapeHandle);
		newShapeHandle=simCreateMeshShape(2,20.0f*piValue/180.0f,vertices,verticesSize,indices,indicesSize,NULL);
		simReleaseBuffer((char*)vertices);
		simReleaseBuffer((char*)indices);
	}
	return(newShapeHandle);
}


