//Need to update this to draw a new graph


 $(".UpdateButton").click(update);
function update(){
	var Kind = $(this).attr('id');
	if (Kind == "update real-time"){
        $.getJSON("http://localhost:3001/real", function(data,status){
			var INPUT = $('#realtime_temperature');
        	INPUT.val(data.real);
		});
	}
	else if (Kind == "update max"){
        $.getJSON("http://localhost:3001/max", function(data,status){
			var INPUT = $('#max_temperature');
        	INPUT.val(data.max);
		});
		
	}
	else if(Kind == "update min") {
        $.getJSON("http://localhost:3001/min", function(data,status){
			var INPUT = $('#min_temperature');
        	INPUT.val(data.min);
		});
	}
	else {
		$.getJSON("http://localhost:3001/average", function(data,status){
			var INPUT = $('#aver_temperature');
			INPUT.val(data.average);
		});
	}
}

$("#UpdateAll").click(updateall);

function updateall(){
	$.getJSON("http://localhost:3001/updateall", function(data,status){
		$('#aver_temperature').val(data.average);
		$('#min_temperature').val(data.min);
		$('#max_temperature').val(data.max);
		$('#realtime_temperature').val(data.real);
	});
}

function Draw_graph2(){


	$.getJSON("http://localhost:3001/graph", function(data,status){
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
	});

}

$("#DrawGraph").click(Draw_graph2);

/*
//Celsius and Fahrenheit
var Tem_type=0; //0:C 1:F
$("#Fahrenheit").click(ToFahrenheit);
$("#Celsius").click(ToCelsius);

function ToFahrenheit(){
	if(Tem_type==0){
		Tem_type=1;
		Temp.max=Temp.max*1.8+32; Temp.min=Temp.min*1.8+32; Temp.real=Temp.real*1.8+32; Temp.average=Temp.average*1.8+32;
		//if ($("#realtime_temperature").val() != "") $("#realtime_temperature").val(Temp.real);
	}
}

function ToCelsius(){
	if(Tem_type==1){
		Tem_type=0;
		Temp.max=(Temp.max-32)/1.8; Temp.min=(Temp.min-32)/1.8; Temp.real=(Temp.real-32)/1.8; Temp.average=(Temp.average-32)/1.8; 
		//if ($("#realtime_temperature").val() != "") $("#realtime_temperature").val(Temp.real);
	}
}
*/