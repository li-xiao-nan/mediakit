#ifndef BASE_MACROS_H_
#define BASE_MACROS_H_

#define DISALLOW_COPY(TypeName) \
  TypeName (const TypeName&) = delete

#define DISALLOW_ASSIGN(TypeName) \
  TypeName& operator = (const TypeName&) = delete

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  DISALLOW_COPY(TypeName); \
  DISALLOW_ASSIGN(TypeName)
#endif