#ifndef _FCNAV_MySharedPtr_H
#define _FCNAV_MySharedPtr_H


template <class T>
class MySharedPtr
{


public:
 //constructor creates the dynamic interger to hold ref counting
 explicit MySharedPtr(T* p = 0):m_ptr(p)
 {
  pRefCount = new int();
  *pRefCount = 1;
 }


 ~MySharedPtr()
 {
 // std::cout << "inside destructor refCount = " << *pRefCount << "\n";
	 //destroy only if ref count is 1 else null set member
  if(1 == *pRefCount)
  {
   if(NULL != m_ptr)
    delete m_ptr;
   delete pRefCount;
  }
  else
  {
   m_ptr = NULL;
   //decrement the counter when not deleteing object
   --*pRefCount;
  }
  
 }


MySharedPtr(const MySharedPtr<T> & rhs)
 {
  //during copy construction share the object pointer
  // and ref counting pointer
  this->m_ptr = rhs.m_ptr;
  this->pRefCount = rhs.pRefCount;
  //increment the counter as we have one more object now
  ++*pRefCount;
 }


 MySharedPtr<T>& operator = (const MySharedPtr<T>& rhs)
 {
  if(this->m_ptr != rhs.m_ptr)
  {
   if(this->m_ptr != NULL)
   {
    //if rhs object counter is 1 then delete it otherwise
    //decrement rhs counter
    if(*this->pRefCount == 1)
    {
     delete m_ptr;
     m_ptr = NULL;
     delete pRefCount; 
     pRefCount = NULL;
    }
    else
    {
     --*(this->pRefCount);
    }
   }
   //assing rhs and increment counter
   this->m_ptr = rhs.m_ptr;
   this->pRefCount = rhs.pRefCount;
   ++*(rhs.pRefCount);
  }
  return *this;
 }

  T* operator->() const { return m_ptr; }
  T& operator*() const { return *m_ptr; }

 T* get(){return m_ptr;}
private:
 T* m_ptr;
 //pointer to hold ref counter
 int* pRefCount;


};

#endif