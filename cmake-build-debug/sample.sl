module sample
    private class A inherits Object
        public field-array[11] character str := "test";
        public field-array[2] array[3] array[4] integer arr3d;
        public field-integer i := 1;
        private field-integer ip := 11;
        public method p1(A a)(in integer i2, in float f): integer
            output "A class\ni2 = ";
            output i2;
            output ", f = ";
            output f;
            output "\n\n";
            return i2;
        end p1;
    end A;

    private procedure printHelloProc(in array[] array[] character word)
        output "Hello, ";
	    output word;
    end printHelloProc;

    private function printHelloFunc(in array[] array[] character word): array[] array[] character
        output "Hello, ";
	    output word;
	    return word;
    end printHelloFunc;

    private function getFloat(): float
        return 3.14f;
    end getFloat;

start
    variable-array[2] array[3] array[4] function(in integer, in integer): integer arr3d2;
    output arr3d2;
    // test line comment
    variable-real r1 := 3.14;
    variable-real r2 := 1 + r1;
    variable-xdd.Xdd xdd := 1;
    variable-xddd xdd := 1;
    variable-integer i := 1.0f;
    variable-procedure(in integer, in integer) procPointer := testProc;
    variable-procedure(in integer) procPointer2 := testProc;
    variable-procedure(in real) procPointer3 := testProc;
    variable-procedure() procPointer4 := testProc;
    variable-procedure(out function(in integer, in integer): integer) procPointer5 := testProc;

    output xdd.xdd1.xdd2;
    while xdd == ad(aaa).a[5] && gg != 5 repeat
        call printHello("aaa");
        variable-xdd.Xdd xdd := 1;
        variable-Xdd xdd := 1;
        input dada;
        output dadada;
        variable-array[2] array[3] array[4] integer arr3d2;
        variable-array[xdd] xdd2.aa arr3d2;
        let arr3d2[xdd().xdd()][2][3] := 1;
        variable-integer i := arr[xdd](22);
        variable-boolean fafaf := f(1, 2.1f, fa(T.a).test).tst().test().test(test());
        variable-integer i := 2 + (1 + 4) * 2;      // test trailing comment
        variable-real r := 3.1415 * 2;
        variable-float f := -3.14f;
        variable-boolean b := true && false;
        variable-boolean b2 := 1 != 2;
    end while;

    if xdd != aaaa then
        output "true1";
        output "true2";
    elseif xdd == aaa + 1 then
        output "elseif11";
        output "elseif12";
    elseif xdd == aaa + 2 then
        output "elseif21";
        output "elseif22";
    else
        output "false1";
        output "false2";
    end if;
end sample.