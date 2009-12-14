// RUN: clang -cc1 -triple i386-apple-darwin9 -emit-llvm %s -o - | FileCheck %s

@class  Some;

@protocol Proto
- (id)initSome:(Some *)anArg;
@end


@interface Table <Proto>
@end

@interface BetterTable: Table

- (id)initSome:(Some *)arg;

@end

@implementation BetterTable

- (id)initSome:(Some *)arg {

 if(self=[super initSome:arg])
 {
	;
 }
// CHECK: load %struct._objc_class** getelementptr inbounds (%struct._objc_class* @"\01L_OBJC_CLASS_BetterTable", i32 0, i32 1)

 return self;
}
@end

