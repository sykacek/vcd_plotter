`timescale 1ns / 1ns
module app_tb ();
	reg A;
	reg B;
	initial begin
		$dumpfile("app.vcd");
		$dumpvars(0, app_tb);
	end


	initial begin
		for(integer i = 0; i < 2; i = i + 1) begin
			A <= 0;
			B <= 0;
			#10
			B <= 1;
			#10
			A <= 1;
			B <= 0;
			#10
			B <= 1;
			#10
			A <= 0;
		end

		#10 $finish;
	end


endmodule 
