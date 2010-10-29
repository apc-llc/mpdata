//************************************************************************
//																		*
//		     Copyright (C)  2008										*
//     University Corporation for Atmospheric Research					*
//		     All Rights Reserved										*
//																		*
//************************************************************************/
//
//	File:		ParamsBase.h
//
//	Author:		John Clyne, modified by Alan Norton
//			National Center for Atmospheric Research
//			PO 3000, Boulder, Colorado
//
//	Date:		March 2008
//
//	Description:	
//		Defines the ParamsBase class
//		This is an abstract class for all classes that rely on 
//		accessing an XML node for get/set
//

//
//! \class ParamsBase
//! \brief Nodes with state in Xml tree representation
//! \author John Clyne
//! \version $Revision: 1.3 $
//! \date    $Date: 2008/04/10 19:47:31 $
//!
//! This is abstract parent of Params and subclasses with state 
//! kept in an xml node.  Used with the ParamNode class to support
//! user-defined Params classes.
//! 
//! Users can extend ParamsBase classes to include arbitrary
//! child (sub) nodes, by support parsing of such nodes as well
//! as setting a dirty flag on such a node when the underlying
//! data is changed.  This is useful for adding transfer 
//! functions to a Params class.  Users can do the following
//! to make use of this extension:
//!
//! 1.  Implement elementStartHandler() and elementEndHandler()
//!		in the ParamsBase extension class to parse the sub nodes of the
//!     node.  During this parsing, be sure to add the appropriate nodes
//!		to the retained XmlTree, rooted at _rootParamNode.  Any
//!		data structures and classes associated with the sub node should
//!		be constructed during parsing.
//!	2.  The classes that are associated with sub nodes should be derived
//!		from ParsedXml.  These classes should implement elementStartHandler()
//!		and elementEndHandler so they can do their own parsing.
//! 3.  Classes that are associated with sub nodes should implement the 
//!		buildNode() method.  buildNode() will build an XML node for the sub node,
//!     and must be called from the Params buildNode() method.
//! 4.  Whenever data within a subnode changes, an appropriate dirty flag associated with
//!     the node should be set in the parent Params node.
//! 5.  Client classes (e.g. renderers) that are interested in changes to the state of the
//!		sub node must call ParamNode::RegisterDirtyFlag*(), and respond
//!		appropriately to the resulting DirtyFlag::Set().

#ifndef ParamsBase_H
#define ParamsBase_H

#include "vapor/XmlNode.h"
#include "vapor/ExpatParseMgr.h"
#include "assert.h"
#include "vaporinternal/common.h"


using namespace VetsUtil;

namespace VAPoR{

class XmlNode;
class ParamNode;

class PARAMS_API ParamsBase : public ParsedXml {
	
public: 
ParamsBase(
	XmlNode *parent, const string &name
 );
//Default constructor for Params that don't (yet) use this class's capabilities
ParamsBase() {
	_currentParamNode = _rootParamNode = 0;
	_parseDepth = 0;
}
 virtual ~ParamsBase();

 //! Set the parent node of the XmlNode tree.
 //!
 //! Sets a new parent node for the XmlNode tree parameter 
 //! data base. The parent node, \p parent, must have been
 //! previously initialized by passing it as an argument to
 //! the class constructor ParamBase(). This method permits
 //! wholesale changing of the entire parameter database.
 //!
 //! \param[in] parent Parent XmlNode.
 //
 void SetParent(XmlNode *parent);


 //! Xml start tag parsing method
 //!
 //! This method is called to handle parsing of an XML file. The contents
 //! of the file will replace any current parameter settings. The method
 //! is virtual so that derived classes may receive notification when
 //! an object instance is reseting state from an XML file
 //!
 //! May be overridden if the class variable names are not determined from
 //! the xml representation of the class
 //
 virtual bool elementStartHandler(
	ExpatParseMgr* pm, int depth, string& tag, const char ** attribs
 );

 //! Xml end tag parsing method
 //!
 //! This method is called to handle parsing of an XML file. The contents
 //! of the file will replace any current parameter settings. The method
 //! is virtual so that derived classes may receive notification when
 //! an object instance has finished reseting state from an XML file.
 //! Must be overridden if the class variable names are not determined from
 //! the xml representation of the class
 //
 virtual bool elementEndHandler(ExpatParseMgr* pm, int depth, string& tag);

 //! Return the top (root) of the parameter node tree
 //!
 //! This method returns the top node in the parameter node tree
 //!

ParamNode *GetRootNode() { return(_rootParamNode); }

//!	
//! Methods to build an xml node from state.
//!

virtual XmlNode* buildNode() { return 0;}

//!	
//! Method for manual setting of node flags
//!

void SetFlagDirty(const string& flag);

virtual void restart() = 0;
protected:
	
	ParamNode *_currentParamNode;
	ParamNode *_rootParamNode;

	int _parseDepth;


protected:

 //! Return the current node in the parameter node tree
 //!
 //! This method returns the current node in the parameter node tree. 
 //! \sa Push(), Pop()
 //!
 ParamNode *GetCurrentNode() { return(_currentParamNode); }

 //! Move down a level in the parameter tree
 //!
 //! The underlying storage model for parameter data is a hierarchical tree. 
 //! By default the hierarchy is flat. This method can be used to add
 //! and navigate branches of the tree. Invoking this method makes the branch
 //! named by \p name the current branch (node). If the branch \p name does not
 //! exist it will be created with the name provided. 
 //! Subsequent set and get methods will operate
 //! relative to the current branch.
 //! User-specific subtrees can be provided by extending this method
 //!
 //! \param[in] name The name of the branch
 //! \retval node Returns the new current node
 //!
 //! \sa Pop(), Delete(), GetCurrentNode()
 //
 ParamNode *Push(
	 string& tag
	);


 //! Move up one level in the paramter tree
 //!
 //! This method move back up the tree hierarchy by one level.
 //! Moving up past the root of the tree is prohibited and will silenty fail
 //! with no ill effects.
 //!
 //! \retval node Returns the new current node
 //!
 //! \sa Pop(), Delete()
 //
 ParamNode *Pop();

 //! Delete the named branch.
 //!
 //! This method deletes the named child, and all decendents, of the current 
 //! destroying it's contents in the process. The 
 //! named node must be a child of the current node. If the named node
 //! does not exist the result is a no-op.
 //!
 //! \param[in] name The name of the branch
 //
 void Remove(const string &name);

 //! Return the attributes associated with the current branch
 //!
 //! \retval a list of attributes
 //
 const map <string, string> &GetAttributes();


 //! Remove (undefine) all parameters
 //!
 //! This method deletes any and all paramters contained in the base 
 //! class as well as deleting any tree branches.
 //
 void Clear();


};


}; //End namespace VAPoR
#endif //ParamsBase_H
