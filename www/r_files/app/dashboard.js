
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
var aux_chart1 = {
		view: "chart",
		id: "aux_chart1",
		type: "line",
		xAxis:{
			template: "#ind1#"
		},
		//tooltip: {
		//	template: "#number#M $"
		//},
		//minHeight:140,
		
	/*	legend:{
			layout:"x",
			width:100,
			align:"right",
			valign:"top",
			values:[
				{text:"Ext1 Ozone",color:"#61b5ee"},
				{text:"Ext2 OA",color:"#e9df40"},
			
			]
		},*/
		
		yAxis:{
			start:0,
			end: 100,
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
           
		],
		padding:{
			top: 25
		},
		data:[]
};
var aux_chart2 = {
		view: "chart",
		id: "aux_chart2",
		type: "line",
		xAxis:{
			template: "#ind2#"
		},
		//tooltip: {
		//	template: "#number#M $"
		//},
		//minHeight:140,
		
	/*	legend:{
			layout:"x",
			width:100,
			align:"right",
			valign:"top",
			values:[
				{text:"Ext1 Ozone",color:"#61b5ee"},
				{text:"Ext2 OA",color:"#e9df40"},
			
			]
		},*/
		
		yAxis:{
			start:0,
			end: 100,
			step: 20
		},
		offset: false,
		series:[
			{
				value: "#aux2_val#",


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

var bar_chart_ext1={
    id:"bar_chart_ext1",
    view: "chart",
    type: "bar",
    value: "#val_ext1#",
    label: "#val_ext1#",
    color: "#color_ext1#",
    radius: 0,
    barWidth: 40,
    tooltip: {
        template: "#val_ext1#"
    },
    xAxis: {
       // title: "Sales per year",
        template: "'#vname_ext1#",
        lines: false
    },
	/* yAxis:{
          //  start:0,
            end:100,
          //  step:10,
          //  title:"External 2"
     },*/
    padding: {
        left: 10,
        right: 10,
        top: 50
    },
    data: []

};
var bar_chart_ext2={
    id:"bar_chart_ext2",
    view: "chart",
    type: "bar",
    value: "#val_ext2#",
    label: "#val_ext2#",
    color: "#color_ext2#",
    radius: 0,
    barWidth: 40,
    tooltip: {
        template: "#val_ext2#"
    },
    xAxis: {
       // title: "Sales per year",
        template: "'#vname_ext2#",
        lines: false
    },
	/* yAxis:{
            //start:0,
            end:100,
            //step:10,
          //  title:"External 2"
     },*/

    padding: {
        left: 10,
        right: 10,
        top: 50
    },
    data: []

};

//==========================================================================================
var col_bar_chart_ext1=
{
								type: "clean",
								rows:[
									{
										"template": "<span class='mkrwa_icon fa-area-chart'></span>Ext1 bar chart", "css": "sub_title", "height": 30
									},
									bar_chart_ext1//aux2_chart
								]
							};
var col_bar_chart_ext2=
{
								type: "clean",
								rows:[
									{
										"template": "<span class='mkrwa_icon fa-area-chart'></span>Ext2 bar chart", "css": "sub_title", "height": 30
									},
									bar_chart_ext2//aux2_chart
								]
							};
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
							},
							{
								//gravity: 4,
								type: "clean",
								rows:[
									{
										"template": "<span class='mkrwa_icon fa-area-chart'></span>VOC-CO2", "css": "sub_title", "height": 30
									},
									voc_co2__chart
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
								cols:[
									col_bar_chart_ext1,col_bar_chart_ext2
								]
							},
							{
								//gravity: 4,
								type: "clean",
								rows:[
									{
										"template": "<span class='mkrwa_icon fa-area-chart'></span>External 1", "css": "sub_title", "height": 30
									},
									aux_chart1
								]
							},
							{
								//gravity: 4,
								type: "clean",
								rows:[
									{
										"template": "<span class='mkrwa_icon fa-area-chart'></span>External 2", "css": "sub_title", "height": 30
									},
									aux_chart2
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
function updScale(n,v)
{
	var x=$$(n);
	x.define({yAxis:{
			start:0,
			end: v,
			step: (v/5).toFixed()
		}});  //define({end:1000});
	x.refresh();
}
function updScaleB(n,v)
{
	var x=$$(n);
	x.define({yAxis:{
			start:0,
			end: v,
			step: (v/5).toFixed(),
			color:"#ffffff",
			lineColor:"#ffffff",
			template: function(value){
				return ""
			}
		}});  //define({end:1000});
	x.refresh();
}
//var _aux1=0,_aux2=0;
function updateChart(d,a)  // a - asset
{
    var co2_exist = 1;
	var cf=a.dconf.config,tn="T (°C)",cr,t,rh,flag,v,C,V,x;
	cr=cf.corr;
	t=-39.66 + 0.01 *d.mT1,
	rh=-2.0468 + 0.0367 * d.mH1 + (-1.5955 * 0.000001) * d.mH1 * d.mH1,flag=cf.ip_flag;
	t -= 4.0;  // Sam request
	t+=cr.temp;

//fake
//_aux1+=10,_aux2+=10;
//d.aux1+=_aux1;
//d.aux2+=_aux2;

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
	
	
	var range1=cf.ext1_sensor.max_value-cf.ext1_sensor.min_value;
	var ozf= flag & 0x80,aff=flag&0x100,metric=flag&0x200,
	//aux1_n="Ext1 ( % )",
	aux1_n=cf.ext1_sensor.sensor_name +" ( "+cf.ext1_sensor.sensor_unit+" )";
	//aux1 = d.aux1 * 1.0 * cf.aux1_range / 1024.0,
	aux1 = d.aux1 * 1.0 * range1 / 1024.0,
    aux2_n="Ext2 ( % )",aux2 = d.aux2 * 1.0 * cf.aux2_range / 1024.0;
	aux1+=cr.Oz;
	aux2+=cr.CO; // must be changed to ??
	/*
	if(ozf>0){
	 aux1_n="Ozone(ppb)";
	 
	}*/
	
	updScale("aux_chart1",/*cf.aux1_range*/range1);
	 if(aff>0)
	 {
		 aux2_n="Air flow (CFM)";
		 if(metric>0)
		   aux2_n="Air flow (CMH)";
	 }
	
	if(cf.version>192)
	{
	    if ((flag & 0x10) == 0) { C = 0; co2_exist = 0; }
		if((flag&0x20)==0){ aux1=0;aux1_n="";}
		if((flag&0x40)==0){ aux2=0;aux2_n="";}
	}
	else{
		var copt=a.dconf.sensor_options;
		if ((copt & 1) == 0) { C = 0; co2_exist = 0; }
		if((copt&2)==0){aux1=0;aux1_n="";}
		if((copt&4)==0){aux2=0;aux2_n="";}
	}
	
	
	v={ voc_v:V, co2_v:C, ind: c_ind};
	addWithCheck("voc_co2_id",v);
	
	
	 
	 updScale("aux_chart1",cf.aux1_range);
	 updScale("aux_chart2",cf.aux2_range);
	
	v = { aux1_val: aux1, ind1: c_ind };
	addWithCheck("aux_chart1",v);
	
	v = { aux2_val: aux2, ind2: c_ind };
	addWithCheck("aux_chart2",v);
	
	
	//v={ number:d.aux2, ind: c_ind};
	//addWithCheck("aux2_chart",v);
	
	c_ind++;

    //update bar chart
	x = $$("bar_chart");
	x.clearAll();
	
	x.add({ id: 1, val: t.toFixed(), vname:tn , color: "#ee4339" });
	x.add({ id: 2, val: rh.toFixed(), vname: "RH", color: "#ee9336" });
	x.add({ id: 3, val: V, vname: "VOC", color: "#eed236" });
	if (co2_exist)
	    x.add({ id: 4, val: C, vname: "CO2", color: "#d3ee36" });
	
	//x.add({ id: 5, val: aux1.toFixed(), vname: aux1_n, color: "#a7ee70" });
	//x.add({ id: 6, val: aux2.toFixed(), vname: aux2_n, color: "#58dccd" });
	
	updScaleB("bar_chart_ext1",cf.aux1_range);
	updScaleB("bar_chart_ext2",cf.aux2_range);
	x= $$("bar_chart_ext1");
	x.clearAll();
	var _ext1=aux1.toFixed();
	if(_ext1<=1)
		_ext1=0;
	x.add({ id: 1, val_ext1: _ext1, vname_ext1: aux1_n, color_ext1: "#a7ee70" });
	x= $$("bar_chart_ext2");
	x.clearAll();
	x.add({ id: 1, val_ext2: aux2.toFixed(), vname_ext2: aux2_n, color_ext2: "#58dccd" });
	

}
function cleanUpDB()
{
	$$("th_chart_id").clearAll();
	$$("voc_co2_id").clearAll();
	$$("aux_chart1").clearAll();
	$$("aux_chart2").clearAll();
	$$("bar_chart").clearAll();
	
	$$("bar_chart_ext1").clearAll();
	$$("bar_chart_ext2").clearAll();
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