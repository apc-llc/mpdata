//
//      $Id: XmlNode.h,v 1.14 2008/06/16 22:24:33 clynejp Exp $
//

#ifndef	_XmlNode_h_
#define	_XmlNode_h_

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <vapor/MyBase.h>
#ifdef WIN32
#pragma warning(disable : 4251)
#endif

namespace VAPoR {


//
//! \class XmlNode
//! \brief An Xml tree
//! \author John Clyne
//! \version $Revision: 1.14 $
//! \date    $Date: 2008/06/16 22:24:33 $
//!
//! This class manages an XML tree. Each node in the tree
//! coresponds to an XML "parent" element. The concept
//! of "parent" element is a creation of this class,
//! and should be confused with any notions of parent
//! in more commonly used XML jargon. A parent element
//! is simply one possessing child XML elements 
//! Non-parent elements - those elements that do not have 
//! children elements -
//! may be thought of as data elements. Typically
//! non-parent elements possess XML character data. Parent
//! elements may or may not have XML character data 
//! themselves.
//!
//! The XmlNode class is derived from the MyBase base
//! class. Hence all of the methods make use of MyBase's
//! error reporting capability - the success of any method
//! (including constructors) can (and should) be tested
//! with the GetErrCode() method. If non-zero, an error 
//! message can be retrieved with GetErrMsg().
//!
class VDF_API XmlNode : public VetsUtil::MyBase {
public:
	enum ErrCode_T {
		ERR_DEF = 1,	// default error
		ERR_TNP			// Tag not present
	};


 //! Constructor for the XmlNode class.
 //!
 //! Create's a new Xml node 
 //!
 //! \param[in] tag Name of Xml node
 //! \param[in] attrs A list of Xml attribute names and values for this node
 //! \param[in] numChildren Reserve space for the indicated number of 
 //! children. Children must be created with
 //! the NewChild() method
 //!
 XmlNode(
	const string &tag, const map<string, string> &attrs, 
	size_t numChildrenHint = 0
 );

 virtual XmlNode *Construct(
	const string &tag, const map<string, string> &attrs, 
	size_t numChildrenHint = 0
 ) {return(new XmlNode( tag, attrs, numChildrenHint)); }

 //! Copy constructor for the XmlNode class.
 //!
 //! Create's a new XmlNode node from an existing one.
 //!
 //! \param[in] node XmlNode instance from which to construct a copy
 //! 
 XmlNode(const XmlNode &node);

 virtual XmlNode *Clone() {return new XmlNode(*this); };

 virtual ~XmlNode();

 //! Set or get that node's tag (name)
 //!
 //! \retval tag A reference to the node's tag
 //
 string &Tag() { return (_tag); }

 //! Set or get that node's attributes
 //!
 //! \retval attrs A reference to the node's attributes
 //
 map <string, string> &Attrs() { return (_attrmap); }

 // These methods set or get XML character data, possibly formatting
 // the data in the process. The paramter 'tag' identifies the XML
 // element tag associated with the character data. The
 // parameter, 'values', contains the character data itself. The Long and 
 // Double versions of these methods convert a between character streams
 // and vectors of longs or doubles as appropriate. 
 //
 // Get methods return a negative result if the named tag does not exist.
 // If the named tag does not exist, 'Get' methods will return a 
 // reference to a vector (or string) of zero length, *and* will
 // register an error with the SetErrMsg() method.
 //

 //! Set an Xml element of type long
 //!
 //! This method defines and sets an Xml element. The Xml character 
 //! data to be associated with this element is the array of longs
 //! specified by \p values
 //! 
 //! \param[in] tag Name of the element to define/set
 //! \param[in] values Vector of longs to be converted to character data
 //!
 //! \retval status Returns a reference to vector containing the data
 //
 virtual vector<long> &SetElementLong(
	const string &tag, const vector<long> &values
 );

 //! Get an Xml element's data of type long
 //!
 //! Return the character data associated with the Xml elemented 
 //! named by \p tag for this node. The data is interpreted and 
 //! returned as a vector of longs. If the element does not exist
 //! an empty vector is returned
 //!
 //! \param[in] tag Name of element
 //! \retval vector Vector of longs associated with the named elemented
 //!
 virtual vector<long> &GetElementLong(const string &tag);

 //! Return true if the named element of type long exists
 //!
 //! \param[in] tag Name of element
 //! \retval bool 
 //!
 virtual int HasElementLong(const string &tag) const;

 //! Set an Xml element of type double
 //!
 //! This method defines and sets an Xml element. The Xml character 
 //! data to be associated with this element is the array of doubles
 //! specified by \p values
 //! 
 //! \param[in] tag Name of the element to define/set
 //! \param[in] values Vector of doubles to be converted to character data
 //!
 //! \retval status Returns a reference to vector containing the data
 //
 virtual vector<double> &SetElementDouble(
	const string &tag, const vector<double> &values
 );

 //! Get an Xml element's data of type double
 //!
 //! Return the character data associated with the Xml elemented 
 //! named by \p tag for this node. The data is interpreted and 
 //! returned as a vector of doubles. If the element does not exist
 //! an empty vector is returned
 //!
 //! \param[in] tag Name of element
 //! \retval vector Vector of doubles associated with the named elemented
 //!
 virtual vector<double> &GetElementDouble(const string &tag);

 //! Return true if the named element of type double exists
 //!
 //! \param[in] tag Name of element
 //! \retval bool 
 //!
 virtual int HasElementDouble(const string &tag) const;

 //! Set an Xml element of type string
 //!
 //! This method defines and sets an Xml element. The Xml character 
 //! data to be associated with this element is the array of characters
 //! specified by \p values
 //! 
 //! \param[in] tag Name of the element to define/set
 //! \param[in] values Vector of characters to be converted to character data
 //!
 //! \retval status Returns a non-negative value on success
 //! \retval status Returns a reference to string containing the data
 //
 virtual string &SetElementString(const string &tag, const string &str);

 //! Get an Xml element's data of type string
 //!
 //! Return the character data associated with the Xml elemented 
 //! named by \p tag for this node. The data is interpreted and 
 //! returned as a string. If the element does not exist
 //! an empty vector is returned
 //!
 //! \param[in] tag Name of element
 //! \retval string The string associated with the named element
 //!
 virtual string &GetElementString(const string &tag);

 //! Set an Xml element of type string
 //!
 //! This method defines and sets an Xml element. The Xml character 
 //! data to be associated with this element is the array of strings
 //! specified by \p values. The array of strings is first
 //! translated to a single string of space-separated words (contiguous
 //! characters)
 //! 
 //! \param[in] tag Name of the element to define/set
 //! \param[in] values Vector of strings to be converted to a
 //! space-separated list of characters
 //!
 //! \retval status Returns a reference to string containing the data
 //
 virtual string &SetElementStringVec(
	const string &tag,const vector <string> &strvec
 );

 //! Get an Xml element's data of type string
 //!
 //! Return the character data associated with the Xml elemented 
 //! named by \p tag for this node. The data is interpreted as
 //! a space-separated list of words (contiguous characters). The
 //! string vector returned is generated by treating white
 //! space as delimeters between vector elements. 
 //! If the element does not exist
 //! an empty vector is returned
 //!
 //! \param[in] tag Name of element
 //! \param[out] vec Vector of doubles associated with the named elemented
 //!
 virtual void GetElementStringVec(const string &tag, vector <string> &vec);

 //! Return true if the named element of type string exists
 //!
 //! \param[in] tag Name of element
 //! \retval bool 
 //!
 virtual int HasElementString(const string &tag) const;

 //! Return the number of children nodes this node has
 //!
 //! \retval n The number of direct children this node has
 //!
 //! \sa NewChild()
 //

 //! Add an existing node as a child of the current node.
 //!
 //! The new child node will be
 //! appended to the array of child nodes.
 //!
 //! \note The node is shallow copied into the tree (only the pointer
 //! is copied. Furthermore, the destructor for this class will delete
 //! the added child. Whoops!!
 //!
 //! \param[in] child is the XmlNode object to be added as a child
 //
 virtual void AddChild(
    XmlNode* child
 );

 virtual int GetNumChildren() const { return (int)(_children.size());};

 //! Create a new child of this node
 //!
 //! Create a new child node, named \p tag. The new child node will be 
 //! appended to the array of child nodes. The \p numChildrenHint
 //! parameter is a hint specifying how many children the new child
 //! itself may have.
 //!
 //! \param[in] tag Name to give the new child node
 //! \param[in] attrs A list of Xml attribute names and values for this node
 //! \param[in] numChildrenHint Reserve space for future children of this node
 //! \retval child Returns the newly created child, or NULL if the child
 //! could not be created
 //
 virtual XmlNode *NewChild(
	const string &tag, const map <string, string> &attrs, 
	size_t numChildrenHint = 0
 );

 //! Delete the indicated child node.
 //! 
 //! Delete the indicated child node, decrementing the total number
 //! of children by one. Return an error if the child does not
 //! exist (i.e. if index >= GetNumChildren())
 //!
 //! \param[in] index Index of the child. The first child is zero
 //! \retval status Returns a non-negative value on success
 //! \sa GetNumChildren()
 virtual int	DeleteChild(size_t index);
 virtual int	DeleteChild(const string &tag);

 //! Replace the indicated child node with specified new child node
 //!
 //! If indicated child does not exist, return -1, otherwise
 //! return the index of the replaced child.
 //!
 //! \param[in] childNode Pointer to existing child node
 //! \param[in] newChild Pointer to replacement child node
 //! \retval status Returns non-negative child index on success

 virtual int    ReplaceChild(XmlNode* childNode, XmlNode* newChild);


 //! Return the indicated child node. 
 //!
 //! Return the ith child of this node. The first child node is index=0,
 //! the last is index=GetNumChildren()-1. Return NULL if the child 
 //! does not exist.
 //!
 //! \param[in] index Index of the child. The first child is zero
 //! \retval child Returns the indicated child, or NULL if the child
 //! could does not exist
 //! \sa GetNumChildren()
 //
 virtual XmlNode *GetChild(size_t index);

 //! Return the node's parent
 //!
 //! This method returns a pointer to the parent node, or NULL if this
 //! node is the root of the tree.
 //!
 //! \retval node Pointer to parent node or NULL if no parent exists
 //
 virtual XmlNode *GetParent() {return(_parent);}

 //! Return true if the indicated child node exists
 //!
 //! \param[in] index Index of the child. The first child is zero
 //! \retval bool 
 //!
 virtual int HasChild(size_t index);

 //! Return the indicated child node. 
 //!
 //! Return the indicated tagged child node. Return NULL if the child 
 //! does not exist.
 //! \param[in] tag Name of the child node to return
 //! \retval child Returns the indicated child, or NULL if the child
 //! could does not exist
 //
 virtual XmlNode *GetChild(const string &tag);

 //! Return true if the indicated child node exists
 //!
 //! \param[in] tag Name of the child node 
 //! \retval bool 
 //!
 virtual int HasChild(const string &tag);

 //! Write the XML tree, rooted at this node, to a file in XML format
 //
 friend ostream& operator<<(ostream &s, const XmlNode& node);

 //Following is a substitute for exporting the "<<" operator in windows.
 //I don't know how to export an operator<< !
 static ostream& streamOut(ostream& os, const XmlNode& node);

private:
 int	_objInitialized;	// has the obj successfully been initialized?

 map <string, vector<long> > _longmap;	// node's long data
 map <string, vector<double> > _doublemap;	// node's double data
 map <string, string> _stringmap;		// node's string data
 map <string, string> _attrmap;		// node's attributes
 vector <XmlNode *> _children;				// node's children
 string _tag;						// node's tag name
 vector <long> _emptyLongVec;				// empty elements 
 vector <double> _emptyDoubleVec;
 vector <string> _emptyStringVec;
 string _emptyString;
 size_t _asciiLimit;	// length limit beyond which element data are encoded
 XmlNode *_parent;	// Node's parent

 // Recursively delete all chidren of the specified node. The node itself
 // is not deleted.
 //
 void _deleteChildren(XmlNode *node);

};
//ostream& VAPoR::operator<< (ostream& os, const XmlNode& node);
};

#endif	//	_XmlNode_h_
