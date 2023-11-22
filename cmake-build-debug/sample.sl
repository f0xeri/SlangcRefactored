module sample
    public function testFunc(in integer x, in integer y): integer
        return x + y;
    end testFunc;

    public procedure testProc(in integer x, in integer y)
        output "testProc(in integer x, in integer y):";
        output x + y;
    end testProc;

    public procedure testProc(out function(in integer, in integer): integer func)
        output "testProc(out function(in integer, in integer): integer func)";
        output func(1, 2);
    end testProc;

    public procedure testProc(in integer x)
        output "testProc(in integer x)";
        output x;
    end testProc;

    public procedure testProc(in real x)
        output "testProc(in real x)";
        output x;
    end testProc;

    public procedure testProc()
        output "testProc()";
    end testProc;

    public procedure testInOutVar(in integer x, out integer y, var integer z, out integer z2)
        output "testInOutVar(in integer x, out integer y, var integer z)";
        output x;
        output y;
        output z;
    end testInOutVar;
start
    variable-procedure(in integer, in integer) procPointer := testProc;
    variable-procedure(in integer) procPointer2 := testProc;
    variable-procedure(in real) procPointer3 := testProc;
    variable-procedure() procPointer4 := testProc;
    variable-procedure(out function(in integer, in integer): integer) procPointer5 := testProc;
    call procPointer(1, 2);
    call procPointer2(1);
    call procPointer3(1.0);
    call procPointer4();
    call procPointer5(testFunc);

    variable-float x := 1.0f;
    variable-integer y := 2;
    variable-integer z := 3;
    call testInOutVar(x, y, z, z);
end sample.