
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
			//title:"Wensday, Oct 24, 2015<br/>&nbsp;",
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
var aux_chart = {
		view: "chart",
		id: "aux_chart",
		type: "line",
		xAxis:{
			template: "#ind#"
		},
		//tooltip: {
		//	template: "#number#M $"
		//},
		//minHeight:140,
		
		legend:{
			layout:"x",
			width:100,
			align:"right",
			valign:"top",
			values:[
				{text:"Ext1 Ozone",color:"#61b5ee"},
				{text:"Ext2 OA",color:"#e9df40"},
			
			]
		},
		
		yAxis:{
			start:0,
			end: 120,
			step: 20
		},
		offset: false,
		series:[
			{
				value: "#aux1_val#",


				item:{
					borderColor: "#fff",
					color: "#61b5ee",
					radius: 4
				},
				line:{
					color:"#61b5ee",
					width:1
				}
			},
            {
                value: "#aux2_val#",


                item: {
                    borderColor: "#fff",
                    color: "#e9df40",
                    radius: 4
                },
                line: {
                    color: "#e9df40",
                    width: 1
                }
            }
		],
		padding:{
			top: 25
		},
		data:[]
};


var bar_chart={
    id:"bar_chart",
    view: "chart",
    type: "bar",
    value: "#val#",
    label: "#val#",
    color: "#color#",
    radius: 0,
    barWidth: 40,
    tooltip: {
        template: "#val#"
    },
    xAxis: {
       // title: "Sales per year",
        template: "'#vname#",
        lines: false
    },
    padding: {
        left: 10,
        right: 10,
        top: 50
    },
    data: []

};

//==========================================================================================
	var dashboard_charts ={
			    margin: 10, rows: [
                 	{
						type: "wide",
						//minHeight: 250,
						cols: [
						    {
								type: "clean",
								rows:[
									{
										"template": "<span class='mkrwa_icon fa-area-chart'></span>Bar chart", "css": "sub_title", "height": 30
									},
									bar_chart//aux2_chart
								]
							},
							{
								//gravity: 4,
								type: "clean",
								rows:[
									{
										"template": "<span class='mkrwa_icon fa-area-chart'></span>Temperature-Humidity", "css": "sub_title", "height": 30
									},
									th_chart
								]
							}
							
						]
					},
					{
						type: "wide",
						//minHeight: 250,
						cols: [
						{
								type: "clean",
								rows:[
									{
									    "template": "<span class='mkrwa_icon fa-area-chart'></span>VOC-CO2", "css": "sub_title", "height": 30
									},
									voc_co2__chart
								]
							},
							{
								//gravity: 4,
								type: "clean",
								rows:[
									{
										"template": "<span class='mkrwa_icon fa-area-chart'></span>Auxiliary inputs", "css": "sub_title", "height": 30
									},
									aux_chart
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
function updateChart(d,a)  // a - asset
{
	var cf=a.dconf.config,tn="T (°C)",cr,t,rh,flag,v,C,V,x;
	cr=cf.corr;
	t=-39.66 + 0.01 *d.mT1,
	rh=-2.0468 + 0.0367 * d.mH1 + (-1.5955 * 0.000001) * d.mH1 * d.mH1,flag=cf.ip_flag;
	t -= 4.0;  // Sam request
	t+=cr.temp;

	if(flag&8)
	{
		tn="T (°F)";t=toFar(t);// farenheit
	}
	
	//t+=cr/temp; // correction
	// if (isFarenheit)
    //            t = Util.DegreeToFarenheit(temp);
	rh += cr.hum;
	rh += 10.0;
	v={ v_T:t, v_H:rh, ind: c_ind};
	addWithCheck("th_chart_id",v);
	C = (d.CO2 * 2.425 + cr.CO2).toFixed();
	V = d.TGS + cr.VOC;
	v={ voc_v:V, co2_v:C, ind: c_ind};
	addWithCheck("voc_co2_id",v);
	
	
	var ozf= flag & 0x80,aff=flag&0x100,metric=flag&0x200,
	aux1_n="Ext1 ( % )",aux1 = d.aux1 * cf.aux1_range / 1024,
    aux2_n="Ext2 ( % )",aux2 = d.aux2 * cf.aux2_range / 1024;
	aux1+=cr.Oz;
	aux2+=cr.CO; // must be changed to ??
	if(ozf>0)
	 aux1_n="Ozone(ppb)";
	 if(aff>0)
	 {
		 aux2_n="Air flow (CFM)";
		 if(metric>0)
		   aux2_n="Air flow (CMH)";
	 }
	
	v = { aux1_val: aux1, aux2_val: aux2, ind: c_ind };
	addWithCheck("aux_chart",v);
	
	//v={ number:d.aux2, ind: c_ind};
	//addWithCheck("aux2_chart",v);
	
	c_ind++;

    //update bar chart
	x = $$("bar_chart");
	x.clearAll();
	
	x.add({ id: 1, val: t.toFixed(), vname:tn , color: "#ee4339" });
	x.add({ id: 2, val: rh.toFixed(), vname: "RH", color: "#ee9336" });
	x.add({ id: 3, val: V, vname: "VOC", color: "#eed236" });
	x.add({ id: 4, val: C, vname: "CO2", color: "#d3ee36" });
	
	x.add({ id: 5, val: aux1.toFixed(), vname: aux1_n, color: "#a7ee70" });
	x.add({ id: 6, val: aux2.toFixed(), vname: aux2_n, color: "#58dccd" });

}
function cleanUpDB()
{
	$$("th_chart_id").clearAll();
	$$("voc_co2_id").clearAll();
	$$("aux_chart").clearAll();
	$$("bar_chart").clearAll();
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
				updateChart(aa.dataArr[i],aa);
			}
			c_ind=i;
		}
	}
}