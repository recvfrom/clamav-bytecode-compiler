// RUN: %clang_cc1 -fsyntax-only -verify %s

typedef struct objc_object {
  Class isa;
} *id;


@interface foo
- (void)meth;
@end

@implementation foo
- (void) contents {}			// No declaration in @interface!
- (void) meth { [self contents]; } 
@end

typedef struct _NSPoint {
    float x;
    float y;
} NSPoint;

typedef struct _NSSize {
    float width; 
    float height;
} NSSize;

typedef struct _NSRect {
    NSPoint origin;
    NSSize size;
} NSRect;

@interface AnyClass
- (NSRect)rect;
@end

@class Helicopter;

static void func(Helicopter *obj) {
  // Note that the proto for "rect" is found in the global pool even when
  // a statically typed object's class interface isn't in scope! This 
  // behavior isn't very desirable, however wee need it for GCC compatibility.
  NSRect r = [obj rect];
}

@interface NSObject @end

extern Class NSClassFromObject(id object);

@interface XX : NSObject 
@end

@implementation XX

+ _privateMethod {
  return self;
}

- (void) xx {
  [NSClassFromObject(self) _privateMethod];
}
@end

@implementation XX (Private)
- (void) yy {
  [NSClassFromObject(self) _privateMethod];
}
@end

@interface I0
-(void) nonVararg: (int) x;
@end

int f0(I0 *ob) {
  [ ob nonVararg: 0, 1, 2]; // expected-error {{too many arguments to method call}}
}

int f2() {
    const id foo;
    [foo bar];  // expected-warning {{method '-bar' not found (return type defaults to 'id')}}
    return 0;
}


// PR3766
struct S { int X; } S;

int test5(int X) {
  int a = [X somemsg];  // expected-warning {{receiver type 'int' is not 'id'}} \
                           expected-warning {{method '-somemsg' not found}} \
                           expected-warning {{incompatible pointer to integer conversion initializing 'id', expected 'int'}}
  int b = [S somemsg];  // expected-error {{bad receiver type 'struct S'}}
}

// PR4021
void foo4() {
  struct objc_object X[10];
  
  [X rect]; // expected-warning {{receiver type 'struct objc_object *' is not 'id' or interface pointer, consider casting it to 'id'}} expected-warning {{method '-rect' not found (return type defaults to 'id')}}
}

