
	var th_chart = {
		view:"chart",
		id:"th_chart_id",
		type: "line",
		offset:0,
		/*
		padding:{
			left:30,
			bottom: 60
		},
		*/
		
		yAxis:{
			/*start:0,
			end: 100,
			step: 10,*/
		},
		xAxis:{
			
			lines:true,
			title:"Wensday, Oct 24, 2015<br/>&nbsp;",
			template:"'#ind#"
		},
		legend:{
			layout:"x",
			width:100,
			align:"right",
			valign:"top",
			values:[
				{text:"Humidity",color:"#61b5ee"},
				{text:"Temperature",color:"#e9df40"},
			
			]
		},
		
		series:[
			
			{
				type:"line",
				value:"#v_T#",
				item:{
					borderColor: "#fff",
					color: "#61b5ee",
					radius: 4
				},
				line:{
					color:"#e9df40",
					width:2
				}
			},
			{
				type:"line",
				value:"#v_H#",
				item:{
					borderColor: "#fff",
					color: "#61b5ee",
					radius: 4
				},
				line:{
					color:"#61b5ee",
					width:2
				}
			}
		],
		data:[]
	};
	

//=============================VOC-CO2=====================================================
     var voc_co2__chart=
				{
					view: "chart",
					type: "line",
					id:"voc_co2_id",
					legend:{
						layout: "x",
                        valign:"top",
						align: "right",
						values: [{text:"VOC", color: "#a1656e"},{text: "CO2", color: "#a4b4bf"}]
					},
					offset:0,
					alpha:0.8,

					xAxis:{
						template: "#ind#"
					},
					radius:0,
					yAxis:{
						start:0,
						end: 2000,
						step: 500
					},
					series:[
						{ 
							value: "#voc_v#", 
							item:{
									borderColor: "#fff",
									color: "#a1656e",
									radius: 4
								},
								line:{
									color:"#a1656e",
									width:2
								}
						},
						{ 
							value: "#co2_v#", 
							item:{
									borderColor: "#fff",
									color: "#a4b4bf",
									radius: 4
								},
								line:{
									color:"#a4b4bf",
									width:2
								}
						}
					],
					padding:{
						top: 25
					},
					data:[]// voc_co2_data
				};
	
//=========================================================================================
var aux1_chart = {
		view: "chart",
		id: "aux1_chart",
		type: "line",
		xAxis:{
			template: "#ind#"
		},
		tooltip: {
			template: "#number#M $"
		},
		minHeight:140,
		yAxis:{
			start:0,
			end: 450,
			step: 150
		},
		offset: false,
		series:[
			{
				value: "#number#",


				item:{
					borderColor: "#fff",
					color: "#61b5ee",
					radius: 4
				},
				line:{
					color:"#61b5ee",
					width:1
				}
			}
		],
		padding:{
			top: 25
		},
		data:[]
	};
	var aux2_chart = {
		view: "chart",
		id: "aux2_chart",
		type: "line",
		xAxis:{
			template: "#ind#"
		},
		tooltip: {
			template: "#number#M $"
		},
		minHeight:140,
		yAxis:{
			start:0,
			end: 450,
			step: 150
		},
		offset: false,
		series:[
			{
				value: "#number#",


				item:{
					borderColor: "#fff",
					color: "#61b5ee",
					radius: 4
				},
				line:{
					color:"#61b5ee",
					width:1
				}
			}
		],
		padding:{
			top: 25
		},
		data:[]
	};


//==========================================================================================
	var dashboard_charts ={
			    margin: 10, rows: [
                 	{
						type: "wide",
						minHeight: 250,
						cols: [
							{
								//gravity: 4,
								type: "clean",
								rows:[
									{
										"template": "<span class='mkrwa_icon fa-area-chart'></span>Temperature/Humidity charts in one", "css": "sub_title", "height": 30
									},
									th_chart
								]
							},
							{
								type: "clean",
								rows:[
									{
										"template": "<span class='mkrwa_icon fa-sign-in'></span>VOC-CO2 charts", "css": "sub_title", "height": 30
									},
									voc_co2__chart
								]
							}
						]
					},
					{
						type: "wide",
						minHeight: 250,
						cols: [
							{
								//gravity: 4,
								type: "clean",
								rows:[
									{
										"template": "<span class='mkrwa_icon fa-area-chart'></span>Aux1 chart", "css": "sub_title", "height": 30
									},
									aux1_chart
								]
							},
							{
								type: "clean",
								rows:[
									{
										"template": "<span class='mkrwa_icon fa-area-chart'></span>Aux2 chart", "css": "sub_title", "height": 30
									},
									aux2_chart
								]
							}
						]
					},
			    ] // rows
			};
			
			
//=======================Update points on charts========================
var c_ind=1;
function addWithCheck(s,v,i)
{
	var x=$$(s);
	if(c_ind>20)
	{
		var z=x.getFirstId(); 
		x.remove(z);
	}
	x.add(v);
}
function updateChart(d)
{
	var t=-39.66 + 0.01 *d.mT1,
	rh=-2.0468 + 0.0367 * d.mH1 + (-1.5955 * 0.000001) * d.mH1 * d.mH1;
	t -= 4.0;  // Sam request
	//t+=cr/temp; // correction
	// if (isFarenheit)
    //            t = Util.DegreeToFarenheit(temp);
	// rh += cr.hum;
    rh += 10.0;
	var v={ v_T:t, v_H:rh, ind: c_ind};
	addWithCheck("th_chart_id",v);
	
	v={ voc_v:d.TGS, co2_v:d.CO2, ind: c_ind};
	addWithCheck("voc_co2_id",v);
	
	v={ number:d.aux1, ind: c_ind};
	addWithCheck("aux1_chart",v);
	
	v={ number:d.aux2, ind: c_ind};
	addWithCheck("aux2_chart",v);
	
	c_ind++;
}
function cleanUpDB()
{
	$$("th_chart_id").clearAll();
	$$("voc_co2_id").clearAll();
	$$("aux1_chart").clearAll();
	$$("aux2_chart").clearAll();
	c_ind=1;
}
function switchUpDB(aa)
{
	cleanUpDB();
	if(aa&&aa.dataArr)
	{
		var len=aa.dataArr.length,i;
		if(len>0)
		{
			for(i=0;i<len;i++)
			{
				updateChart(aa.dataArr[i]);
			}
			c_ind=i;
		}
	}
}