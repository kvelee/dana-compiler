; ModuleID = 'DanaModule'
source_filename = "DanaModule"

@0 = private unnamed_addr constant [13 x i8] c"Moving from \00", align 1
@1 = private unnamed_addr constant [5 x i8] c" to \00", align 1
@2 = private unnamed_addr constant [3 x i8] c".\0A\00", align 1
@numberOfRings = internal global i16 0
@3 = private unnamed_addr constant [8 x i8] c"Rings: \00", align 1
@4 = private unnamed_addr constant [5 x i8] c"left\00", align 1
@5 = private unnamed_addr constant [6 x i8] c"right\00", align 1
@6 = private unnamed_addr constant [7 x i8] c"middle\00", align 1

declare void @writeInteger(i16)

declare void @writeByte(i8)

declare void @writeChar(i8)

declare void @writeString(ptr)

declare i16 @readInteger()

declare i8 @readByte()

declare i8 @readChar()

declare void @readString(i16, ptr)

declare i16 @extend(i8)

declare i8 @shrink(i16)

declare i16 @strlen(ptr)

declare i16 @strcmp(ptr, ptr)

declare void @strcpy(ptr, ptr)

declare void @strcat(ptr, ptr)

define i32 @main() {
entry:
  call void @writeString(ptr @3)
  %0 = call i16 @readInteger()
  store i16 %0, ptr @numberOfRings, align 2
  %numberOfRings_val = load i16, ptr @numberOfRings, align 2
  call void @hanoi(ptr @numberOfRings, ptr @4, ptr @5, ptr @6)
  ret i32 0
}

define void @hanoi(ptr %rings, ptr %source, ptr %target, ptr %auxiliary) {
entry:
  %rings1 = alloca ptr, align 8
  store ptr %rings, ptr %rings1, align 8
  %source2 = alloca ptr, align 8
  store ptr %source, ptr %source2, align 8
  %target3 = alloca ptr, align 8
  store ptr %target, ptr %target3, align 8
  %auxiliary4 = alloca ptr, align 8
  store ptr %auxiliary, ptr %auxiliary4, align 8
  %rings_ref_ptr = load ptr, ptr %rings1, align 8
  %rings_val = load i16, ptr %rings_ref_ptr, align 2
  %cmp_ge = icmp sge i16 %rings_val, 1
  %cond = icmp ne i1 %cmp_ge, false
  br i1 %cond, label %then, label %merge

then:                                             ; preds = %entry
  %rings_ref_ptr5 = load ptr, ptr %rings1, align 8
  %rings_val6 = load i16, ptr %rings_ref_ptr5, align 2
  %subtmp = sub i16 %rings_val6, 1
  %source_ref_ptr = load ptr, ptr %source2, align 8
  %source_val = load i8, ptr %source_ref_ptr, align 1
  %source_ref_ptr7 = load ptr, ptr %source2, align 8
  %auxiliary_ref_ptr = load ptr, ptr %auxiliary4, align 8
  %auxiliary_val = load i8, ptr %auxiliary_ref_ptr, align 1
  %auxiliary_ref_ptr8 = load ptr, ptr %auxiliary4, align 8
  %target_ref_ptr = load ptr, ptr %target3, align 8
  %target_val = load i8, ptr %target_ref_ptr, align 1
  %target_ref_ptr9 = load ptr, ptr %target3, align 8
  call void @hanoi(i16 %subtmp, ptr %source_ref_ptr7, ptr %auxiliary_ref_ptr8, ptr %target_ref_ptr9)
  %source_ref_ptr10 = load ptr, ptr %source2, align 8
  %source_val11 = load i8, ptr %source_ref_ptr10, align 1
  %source_ref_ptr12 = load ptr, ptr %source2, align 8
  %target_ref_ptr13 = load ptr, ptr %target3, align 8
  %target_val14 = load i8, ptr %target_ref_ptr13, align 1
  %target_ref_ptr15 = load ptr, ptr %target3, align 8
  call void @move(ptr %source_ref_ptr12, ptr %target_ref_ptr15)
  %rings_ref_ptr16 = load ptr, ptr %rings1, align 8
  %rings_val17 = load i16, ptr %rings_ref_ptr16, align 2
  %subtmp18 = sub i16 %rings_val17, 1
  %auxiliary_ref_ptr19 = load ptr, ptr %auxiliary4, align 8
  %auxiliary_val20 = load i8, ptr %auxiliary_ref_ptr19, align 1
  %auxiliary_ref_ptr21 = load ptr, ptr %auxiliary4, align 8
  %target_ref_ptr22 = load ptr, ptr %target3, align 8
  %target_val23 = load i8, ptr %target_ref_ptr22, align 1
  %target_ref_ptr24 = load ptr, ptr %target3, align 8
  %source_ref_ptr25 = load ptr, ptr %source2, align 8
  %source_val26 = load i8, ptr %source_ref_ptr25, align 1
  %source_ref_ptr27 = load ptr, ptr %source2, align 8
  call void @hanoi(i16 %subtmp18, ptr %auxiliary_ref_ptr21, ptr %target_ref_ptr24, ptr %source_ref_ptr27)
  br label %merge

merge:                                            ; preds = %then, %entry
  ret void
}

define void @move(ptr %source, ptr %target) {
entry:
  %source1 = alloca ptr, align 8
  store ptr %source, ptr %source1, align 8
  %target2 = alloca ptr, align 8
  store ptr %target, ptr %target2, align 8
  call void @writeString(ptr @0)
  %source_ref_ptr = load ptr, ptr %source1, align 8
  %source_val = load i8, ptr %source_ref_ptr, align 1
  %source_ref_ptr3 = load ptr, ptr %source1, align 8
  call void @writeString(ptr %source_ref_ptr3)
  call void @writeString(ptr @1)
  %target_ref_ptr = load ptr, ptr %target2, align 8
  %target_val = load i8, ptr %target_ref_ptr, align 1
  %target_ref_ptr4 = load ptr, ptr %target2, align 8
  call void @writeString(ptr %target_ref_ptr4)
  call void @writeString(ptr @2)
  ret void
}
