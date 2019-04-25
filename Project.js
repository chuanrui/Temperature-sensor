//Need to update this to draw a new graph
var is_Fahrenheit = -1;
var in_warning = -1;
var standby = -1 ;



setInterval(try_updateall,3000);

function try_updateall(){
	if (standby<0){
		updateall();
		Draw_graph2();
	}
}

$("#stand_by").click(function (){
	if(standby == -1){
		
	    standby = 1; console.log("Now the program is in stand by mode");
	    
	    $.getJSON("http://localhost:3001/stand", function(data,status){
		   	if (data.is_connect<0){
		   		$("#Isconnect").html("Fail to connect to the middleware");
		   	}
		   	else{
		    	//$("#Isconnect").html("Succeed to connect to the middleware");
		    	$("#Isconnect").html("Now the program is in stand by mode");
		   	}
		});
		$('#realtime_temperature').val("----");
     //clearInterval(updateall);
 	}
})

$("#resume").click(function (){
	if (standby == 1){
	standby = -1;
	$.getJSON("http://localhost:3001/resume", function(data,status){
		   	if (data.is_connect<0){
		   		$("#Isconnect").html("Fail to connect to the middleware");
		   	}
		   	else{
		    	$("#Isconnect").html("Succeed to connect to the middleware");
		   	}
		});
	//setInterval(updateall,3000);
	}
})


$("#UpdateAll").click(updateall);


//setInterval(Draw_graph2,3000);

function updateall(){
	$.getJSON("http://localhost:3001/updateall", function(data,status){
		var warning = $("#warning").val();
		if(warning == ""){
			warning = Number.MAX_SAFE_INTEGER;
		}
		else{
			warning = Number(warning);
		}
		if (data.is_connect<0){
        		$("#Isconnect").html("Fail to connect to the middleware");
        }
        else{

        	if (warning < data.real){
        		$("#warningInfo").html("WARNING! The temprature is too high");
        		$.getJSON("http://localhost:3001/warning", function(data,status){
			       	if (data.is_connect<0){
			       		$("#Isconnect").html("Fail to connect to the middleware");
			       	}
			       	else{
			        	$("#Isconnect").html("Succeed to connect to the middleware");
			       	}
				});
        	}
        	else {
        		$("#warningInfo").html("");
    //     		    $.getJSON("http://localhost:3001/normal", function(data,status){
				//        	if (data.is_connect<0){
				//        		$("#Isconnect").html("Fail to connect to the middleware");
				//        	}
				//        	else{
				//         	$("#Isconnect").html("Succeed to connect to the middleware");
				//        	}
				// });
        	}
        	if(is_Fahrenheit>0){
        		$('#aver_temperature').val(Fahrenheit(data.average));
				$('#min_temperature').val(Fahrenheit(data.min));
				$('#max_temperature').val(Fahrenheit(data.max));
				$('#realtime_temperature').val(Fahrenheit(data.real));

				$("#Isconnect").html("Succeed to connect to the middleware");

        	}
        	else{
				$('#aver_temperature').val(data.average);
				$('#min_temperature').val(data.min);
				$('#max_temperature').val(data.max);
				$('#realtime_temperature').val(data.real);
				$("#Isconnect").html("Succeed to connect to the middleware");        		
        	}
			
	    }
	});
	
}

function Draw_graph2(){


	$.getJSON("http://localhost:3001/graph", function(data,status){
		if (data.is_connect<0){
        		$("#Isconnect").html("Fail to connect to the middleware");
        }
        else{
		var Temp_array = data.array;
		var rear=data.rear;
		if (data.full==0){
			Temp_array=Temp_array.slice(0,rear+1);
		}
		else{
			var rear_to_end = Temp_array.slice(rear+1,3600);
			var start_to_rear = Temp_array.slice(0,rear+1);
			var Temp_array=rear_to_end.concat(start_to_rear);
		}


			var option={
		title:{
			text : "Recent Temprature" 
		},

		axisY:{
			title: "Temprature"
		},

		axisX:{

		},
		data:[
			{
				type:"line",
				markerType:"none",
				dataPoints : []
			}
		]
	};
			$.each(Temp_array, function(index, value){
			option.data[0].dataPoints.push({x: index, y: value});
		});
			$("#chartContainer").CanvasJSChart(option);
		}
	});

}

$("#DrawGraph").click(Draw_graph2);


$("#Fahrenheit").click(ToFahrenheit);
$("#Celsius").click(ToCelsius);
$("#Light").click(ChangeLight);
function ToFahrenheit(){
	if(is_Fahrenheit==-1){
		is_Fahrenheit=1;
		var value1 = $('#aver_temperature').val(); $('#aver_temperature').val(Fahrenheit(value1));
		value1 = $('#min_temperature').val();          $('#min_temperature').val(Fahrenheit(value1));
		value1 = $('#max_temperature').val(); 		   $('#max_temperature').val(Fahrenheit(value1));
		value1 = $('#realtime_temperature').val();     $('#realtime_temperature').val(Fahrenheit(value1));
		//if ($("#realtime_temperature").val() != "") $("#realtime_temperature").val(Temp.real);
		$.getJSON("http://localhost:3001/f", function(data,status){
       	if (data.is_connect<0){
       		$("#Isconnect").html("Fail to connect to the middleware");
       	}
       	else{
        	$("#Isconnect").html("Succeed to connect to the middleware");
       	}
	});
	}
}

function ToCelsius(){
	if(is_Fahrenheit==1){
		is_Fahrenheit=-1;
		var value1 = $('#aver_temperature').val();  $('#aver_temperature').val(Celsius(value1));
		 value1 = $('#min_temperature').val();   $('#min_temperature').val(Celsius(value1));
		 value1 = $('#max_temperature').val();   $('#max_temperature').val(Celsius(value1));
		 value1 = $('#realtime_temperature').val();    $('#realtime_temperature').val(Celsius(value1));
		//if ($("#realtime_temperature").val() != "") $("#realtime_temperature").val(Temp.real);
		$.getJSON("http://localhost:3001/c", function(data,status){
       	if (data.is_connect<0){
       		$("#Isconnect").html("Fail to connect to the middleware");
       	}
       	else{
        	$("#Isconnect").html("Succeed to connect to the middleware");
       	}
	});
	}
}

function Fahrenheit(value1){
	return Number(Number(value1)*1.8+32).toFixed(2);
}

function Celsius(value1){
	return Number((Number(value1)-32)/1.8).toFixed(2);
}
function ChangeLight(){
	$.getJSON("http://localhost:3001/light", function(data,status){
       	if (data.is_connect<0){
       		$("#Isconnect").html("Fail to connect to the middleware");
       	}
       	else{
        	$("#Isconnect").html("Succeed to connect to the middleware");
       	}
	});
}