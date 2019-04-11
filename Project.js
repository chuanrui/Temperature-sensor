//Need to update this to draw a new graph
var is_Fahrenheit = -1;

 $(".UpdateButton").click(update);
function update(){

	var Kind = $(this).attr('id');
	if (Kind == "update real-time"){
        $.getJSON("http://localhost:3001/real", function(data,status){
        	if (data.is_connect<0){
        		$("#Isconnect").html("Fail to connect to the middleware");
        	}
        	else{
				var INPUT = $('#realtime_temperature');
				if(is_Fahrenheit>0){
					INPUT.val(Fahrenheit(data.real));
				}
				else{
					INPUT.val(data.real);
				}
	        	
	        	$("#Isconnect").html("Succeed to connect to the middleware");
        	}
		});
	}
	else if (Kind == "update max"){
        $.getJSON("http://localhost:3001/max", function(data,status){
        	if (data.is_connect<0){
        		$("#Isconnect").html("Fail to connect to the middleware");
        	}
        	else{
				var INPUT = $('#max_temperature');
	        	if(is_Fahrenheit>0){
					INPUT.val(Fahrenheit(data.max));
				}
				else{
					INPUT.val(data.max);
				}
	        	$("#Isconnect").html("Succeed to connect to the middleware");
        	}
		});
		
	}
	else if(Kind == "update min") {
        $.getJSON("http://localhost:3001/min", function(data,status){
        	if (data.is_connect<0){
        		$("#Isconnect").html("Fail to connect to the middleware");
        	}
        	else{
				var INPUT = $('#min_temperature');
	        	if(is_Fahrenheit>0){
					INPUT.val(Fahrenheit(data.min));
				}
				else{
					INPUT.val(data.min);
				}
	        	$("#Isconnect").html("Succeed to connect to the middleware");
        	}
		});
	}
	else {
		$.getJSON("http://localhost:3001/average", function(data,status){
			if (data.is_connect<0){
        		$("#Isconnect").html("Fail to connect to the middleware");
        	}
        	else{
				var INPUT = $('#aver_temperature');
				if(is_Fahrenheit>0){
					INPUT.val(Fahrenheit(data.average));
				}
				else{
					INPUT.val(data.average);
				}
				$("#Isconnect").html("Succeed to connect to the middleware");
			}
		});
	}
}

$("#UpdateAll").click(updateall);

function updateall(){
	$.getJSON("http://localhost:3001/updateall", function(data,status){
		if (data.is_connect<0){
        		$("#Isconnect").html("Fail to connect to the middleware");
        }
        else{
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

function ToFahrenheit(){
	if(is_Fahrenheit==-1){
		is_Fahrenheit=1;
		var value1 = $('#aver_temperature').val(); $('#aver_temperature').val(Fahrenheit(value1));
		value1 = $('#min_temperature').val();          $('#min_temperature').val(Fahrenheit(value1));
		value1 = $('#max_temperature').val(); 		   $('#max_temperature').val(Fahrenheit(value1));
		value1 = $('#realtime_temperature').val();     $('#realtime_temperature').val(Fahrenheit(value1));
		//if ($("#realtime_temperature").val() != "") $("#realtime_temperature").val(Temp.real);
	}
}

function ToCelsius(){
	if(is_Fahrenheit==1){
		is_Fahrenheit=-1;
		var value1 = $('#aver_temperature').val();  $('#aver_temperature').val(Celsius(value));
		 value1 = $('#min_temperature').val();   $('#min_temperature').val(Celsius(value));
		 value1 = $('#max_temperature').val();   $('#max_temperature').val(Celsius(value));
		 value1 = $('#realtime_temperature').val();    $('#realtime_temperature').val(Celsius(value));
		//if ($("#realtime_temperature").val() != "") $("#realtime_temperature").val(Temp.real);
	}
}

function Fahrenheit(value1){
	return Number(value1)*1.8+32;
}

function Celsius(value1){
	return (Number(value1)-32)/1.8;
}