/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM ./IXpConn.idl
 */

#ifndef __gen_IXpConn_h__
#define __gen_IXpConn_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

#ifndef __gen_nsIDOMWindow_h__
#include "nsIDOMWindow.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif

/* starting interface:    nsIOhsXpConn */
#define NS_IOHSXPCONN_IID_STR "2077a08d-5d1b-48a4-bd14-99a43ad008f7"

#define NS_IOHSXPCONN_IID \
  {0x2077a08d, 0x5d1b, 0x48a4, \
    { 0xbd, 0x14, 0x99, 0xa4, 0x3a, 0xd0, 0x08, 0xf7 }}

class NS_NO_VTABLE nsIOhsXpConn : public nsISupports {
 public:

  NS_DECLARE_STATIC_IID_ACCESSOR(NS_IOHSXPCONN_IID)

  /* long Add (in long a, in long b); */
  NS_IMETHOD Add(int32_t a, int32_t b, int32_t *_retval) = 0;

  /* long Exec (in ACString p, in ACString q); */
  NS_IMETHOD Exec(const nsACString & p, const nsACString & q, int32_t *_retval) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(nsIOhsXpConn, NS_IOHSXPCONN_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIOHSXPCONN \
  NS_IMETHOD Add(int32_t a, int32_t b, int32_t *_retval) override; \
  NS_IMETHOD Exec(const nsACString & p, const nsACString & q, int32_t *_retval) override; 

/* Use this macro when declaring the members of this interface when the
   class doesn't implement the interface. This is useful for forwarding. */
#define NS_DECL_NON_VIRTUAL_NSIOHSXPCONN \
  NS_METHOD Add(int32_t a, int32_t b, int32_t *_retval); \
  NS_METHOD Exec(const nsACString & p, const nsACString & q, int32_t *_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIOHSXPCONN(_to) \
  NS_IMETHOD Add(int32_t a, int32_t b, int32_t *_retval) override { return _to Add(a, b, _retval); } \
  NS_IMETHOD Exec(const nsACString & p, const nsACString & q, int32_t *_retval) override { return _to Exec(p, q, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIOHSXPCONN(_to) \
  NS_IMETHOD Add(int32_t a, int32_t b, int32_t *_retval) override { return !_to ? NS_ERROR_NULL_POINTER : _to->Add(a, b, _retval); } \
  NS_IMETHOD Exec(const nsACString & p, const nsACString & q, int32_t *_retval) override { return !_to ? NS_ERROR_NULL_POINTER : _to->Exec(p, q, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsOhsXpConn : public nsIOhsXpConn
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIOHSXPCONN

  nsOhsXpConn();

private:
  ~nsOhsXpConn();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS(nsOhsXpConn, nsIOhsXpConn)

nsOhsXpConn::nsOhsXpConn()
{
  /* member initializers and constructor code */
}

nsOhsXpConn::~nsOhsXpConn()
{
  /* destructor code */
}

/* long Add (in long a, in long b); */
NS_IMETHODIMP nsOhsXpConn::Add(int32_t a, int32_t b, int32_t *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* long Exec (in ACString p, in ACString q); */
NS_IMETHODIMP nsOhsXpConn::Exec(const nsACString & p, const nsACString & q, int32_t *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_IXpConn_h__ */
