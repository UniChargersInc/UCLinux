/*
  public class DeviceConfigurationSettings : ICallBack
    {
        public int DeviceID;
        public string dev_payload;
        public int sensor_options, opt1;
        public bool need2send; // Need to Send to Device

        public bool notif_enabled; // notification enabled
        public int interval;       // send Notification interval 1-12hrs,2-24hurs, 3-48 hrs, 4 - one week
        public DateTime NotifTS;   // last time this device email notification has been sent to the customer

    }
	 public enum SensorOptionsEnum
    {  // represents bit number in sensor_options  -> shows if the sensor exists on the board !!!
        CO=1,
        Ozone=2,
        CO2=4,
        Aux1=8  //Aux Input
    }
    */

 function get_dry(cf, ind)
        {
            var i = cf.json_t_outs[ind];
            if (i < 0) i = 0;
            if (i > 10) i = 10;
            return i+1;  // added 1 because ctrl_o value starts with 1
        }
function getBool(v,n)
		{
			return (v&n)==n?true:false;
		}
function toStr(n){return n.toString()}
function showDevConf(a)
{
	//==================================T-RH==================
	var dc=a.dconf,cf=a.dconf.config;  // cf = rTypes.clsCONFIG
	 var s_unit = "°C", isFarV = 2;
    if(/*gO.conf.isFarenheit*/cf.ip_flag&8)  // from client config
    {
        isFarV = 1;
		s_unit = "°F"
    }
	var temp_ui={
		view:"fieldset",label:"Temperature",
		body:{
			rows:[
				//{ template:"Temperature",width:300, type:"section"},
				{ view:"radio",               // should not be editable !!!
				   name:"t_u",value:isFarV,
				   label:"Unit",
				   vertical:true,
					options:[
								{ value:"°F", id:1 },
								{ value:"°C", id:2 }
							] , 
					width:350, 
					align:"left"
				},                       
				{ view:"text",name:"thu",width:350, value:cf.lcdu.TempHiLimit, label:"High Limit ("+s_unit+")" },
				{ view: "text", name: "thl", width: 350, value: cf.lcdu.TempLoLimit, label: "Low Limit (" + s_unit + ")" },
				{ view: "text", name: "tcp", width: 350, value: cf.lcdu.CoolSetPoint, label: "Cooling set point" },
				{ view: "text", name: "thp", width: 350, value: cf.lcdu.HeatSetPoint, label: "Heating set point" },
				{ view:"text",width:350, value:'1', label:"Tolerance plus" },
				{ view:"text", value:'1', label:"Tolerance minus" /*,inputWidth:210*/},  //inputWidth=labelWidth+text input width  !!!
			]
		}//body
	};
	var hum_ui={
	
		view:"fieldset",label:"Humidity",
		
		body:{
			rows:[
				// { template:"***",width:350, type:"section"},
				{margin:30,},
				 { view:"text",name:"hhu",width:350, value:cf.lcdu.HumHiLimit, label:"High Limit " },
				 { view:"text",name:"hhl",width:350, value:cf.lcdu.HumLoLimit, label:"Low Limit " },
				 { view:"text",name:"dsp",width:350, value:cf.lcdu.DehumSetPoint, label:"Dehum set point" },
				{ view:"text",name:"hsp",width:350, value:cf.lcdu.HumSetPoint, label:"Humidity set point" },
				{ view:"text",width:350, value:'1', label:"Tolerance plus" },
				{ view:"text", value:'1', label:"Tolerance minus" /*,inputWidth:210*/},
			]
		}//body
	
	};
	
	  var frm3={
	      view: "form",  id: getFI(3), padding: 18, elements: [{cols:[temp_ui,{ view:"spacer", },hum_ui]}],
			elementsConfig: { labelWidth: 130,inputWidth:210, labelAlign: "left" }
		};
	
	//==================================VOC-CO2==================
	var co2_ui={
		view:"fieldset",label:"VOC",
		body:{
			rows:[
				
				{ view: "text", name: "vor", width: 350, value: cf.lcdu.rVOC, label: "Range (ppm)" },
				{ view: "text", name: "vou", width: 350, value: cf.lcdu.VOC_Hi, label: "Upper level (ppm)" },
				{ view: "text", name: "vol", width: 350, value: cf.lcdu.VOC_Lo, label: "Low level (ppm)" },
				
			]
		}//body
	}; 
	var voc_ui={
	
		view:"fieldset",label:"CO2",
		
		body:{
			rows:[
				// { template:"***",width:350, type:"section"},
				{margin:30,},
				 { view: "text", name: "cor", width: 350, value: cf.lcdu.rCO2, label: "Range (ppm) " },
				 { view: "text", name: "cou", width: 350, value: cf.lcdu.CO2_Hi, label: "Upper level (ppm)" },
				 { view: "text", name: "col", width: 350, value: cf.lcdu.CO2_Lo, label: "Low level (ppm)" },
				
			]
		}//body
	
	};
						
	
	  var frm4={
	      view: "form",  id: getFI(4), padding: 18, elements: [{cols:[voc_ui,{ view:"spacer", },co2_ui]}],
			elementsConfig: { labelWidth: 130,inputWidth:210, labelAlign: "left" }
		};
	
    //=======================================Ext1=======================================================

	  /*var combo_ext1_opt = [
          { id: 1, value: "Methanin" },
          { id: 2, value: "CO - Ext1" },
          { id: 3, value: "O32 - Ext1" },
	  ];*/
	  var combo_ext1_opt = [],Z=gO.ext1_sensors,sn=cf.ext1_sensor.sensor_name;
	   var ext1_opt_ind = 0;
	  if (Z&&Z.length > 0)
	  {
	      for(var i=0;i<Z.length;i++)
	      {
	          var sn1 = Z[i].sensor_name;
	          var q={id:i+1,value:sn1};
	          combo_ext1_opt.push(q);
			  if(sn==sn1)
			    ext1_opt_ind=i+1;
	      }
	  }

	 
	 // var _cb_1 = (cf.ip_flag & 0x80) > 0; // use as ozone sensor
	 var lbl0="Range ("+cf.ext1_sensor.sensor_unit+")";

	var ext1_ui={
		view:"fieldset",label:"Ext1",
		body:{
			rows:[
				
				//{ view: "checkbox", name: "cb_ext1", label: "Use as Ozone", labelWidth: 120, value: _cb_1 ,click:cb_click},
				
                { view: "combo", id: "combo_ext1_id", value: ext1_opt_ind, width: 220, options: combo_ext1_opt, on: { 'onChange': ext1_opt_chgd } },
				{ view: "text",id: "ext1_range_id", name: "ext1_range", width: 370, value: /*cf.aux1_range*/cf.ext1_sensor.max_value, label: lbl0},
				{marginTop:20,},
                { view: "button", type: "iconButton", icon: "sign-out", label: "Add new sensor", click: add_new_ext1_sensor },
			]
		}//body
	};

    //=============================Ext2========================================================
	var _cb_2=(cf.ip_flag& 0x100)>0 , lbl1="Range ( % ) ",lbl2="( mm )";  // use as air flow
	var _cb_3=(cf.ip_flag& 0x200)>0;  // use metric
	var _cb_rad=(cf.ip_flag& 0x400)>0;  // use eadius
	if(_cb_2)
	{
		if(_cb_3)
		{
			lbl1="Range ( cmh ) ";
			
		}
		else
		{ 
		    lbl1="Range ( cfm ) ";
			lbl2="( inch )";
		}
			
	}
	 var combo_air_opt=[
	  	 {id:1, value:"ESF-35-2 Air Flow"},
	  	 {id:2, value:"ESF-35-2 Air Flow - 2"},
	   			
		];
		var combo_air_opt_ind=cf.aux_conf;
		
	var ext2_ui={
	
		view:"fieldset",label:"Ext2",
		
		body:{
			rows:[
				// { template:"***",width:350, type:"section"},
				{margin:30,},
				 { view: "text", id: "ext2_range_id",name: "ext2_range", width: 350, value: cf.aux2_range, label: lbl1 },
				{ view: "checkbox", name: "cb_ext2", label: "Use as Air flow", labelWidth: 120, value: _cb_2 ,click:cb_click1},
				{ view: "checkbox", id: "cb_ext21_id", name: "cb_ext21", label: "Use metric", labelWidth: 120, value: _cb_3, click: cb_click2 },
				{ view:"combo",name:"combo_aif",value:combo_air_opt_ind,width:200, options:combo_air_opt ,on:{'onChange':air_flow_opt_chgd} },
			]
		}//body
	
	};
	var aifc_rect_ui={
		view:"fieldset",label:"Rectangle",id: "view_rect_id",
		body:{
			rows:[
				// { template:"***",width:350, type:"section"},
				{margin:2,},
				{
					
							rows: [
								{ view: "text", id: "d_len_id", name: "d_len", width: 200, labelWidth: 140, value: cf.aux_width, label: "Duct length "+lbl2 },
								{ view: "text", id: "d_hei_id", name: "d_hei", width: 200, labelWidth: 140, value: cf.aux_height, label: "Duct height"+lbl2 },
							]
				}
			]
		}//body
	};
	
	var aifc_circle_ui={
		view:"fieldset",label:"Circle",
		body:{
			rows:[
				// { template:"***",width:350, type:"section"},
				{margin:2,},
				{
					rows:[
							//{ view: "spacer", width: 50 },
							{ view: "text", id: "d_rad_id", name: "d_rad_val", width: 200, labelWidth: 140, value: cf.aux_width, label: "Radius "+lbl2 },
								{ view: "checkbox", name: "cb_rad", label: "Use radius", labelWidth: 160, value: _cb_rad ,click: cb_rad_use_click},
					]
				}
			]
		}//body
	};
	if (!_cb_3)
	    lbl2 = "(inch)";
		 var ctrl_air_ui = {
		     id:"ctrl_air_ui_id",
		     rows: [
                { margin: 19, },
                { template: "Air flow control", width: 400, type: "section" },
                {cols:[aifc_rect_ui,{ view:"spacer", },aifc_circle_ui]},
				{
							cols:[
							{ view: "checkbox", name: "ccb7", label: "Enable air flow control", labelWidth: 160, value: true },
							{ view: "spacer", width: 10 },
							{ view: "button", type: "iconButton", icon: "sign-out", label: "Update air flow range",click: update_airf_range },
							]
				},
				{ view: "spacer", width: 100,height:10 },

		     ]
		 };	
				
	
	  var frm5={
	      view: "form",  id: getFI(5), padding: 18, 
		  elements: [
		  	{
		  		//{cols:[ext1_ui,{ view:"spacer", },ext2_ui]}
				rows: [
                       {cols:[ext1_ui,{ view:"spacer", },ext2_ui]},
                       ctrl_air_ui
					  //{cols:[ctrl_air_ui,{ view:"spacer", },ctrl_air_ui1]},
                    ]
			}
		  ],//elements:
			elementsConfig: { labelWidth: 130,inputWidth:210, labelAlign: "left" }
		};
	
	//*******************************************************************************************************
	
	
	   var ctrl_o=[
	  	 {id:1, value:"None"},
	  	 {id:2, value:"Cooling set point"},
	   	 {id:3, value:"Heating set point"},
		 {id:4, value:"Humidity high limit"},
		 {id:5, value:"Humidity low limit"},
		 {id:6, value:"Ozone high limit"},
		 {id:7, value:"Ozone low limit"},
		 {id:8, value:"VOC high limit"},
		 {id:9, value:"VOC low limit"},
		 {id:10, value:"CO2 high limit"},
		 {id:11, value:"CO2 low limit"},
		 
		 {id:12, value:"Ext1 high limit"},
		 {id:13, value:"Ext1 low limit"},
		 {id:14, value:"Ext2 high limit"},
		 {id:15, value:"Ext2 low limit"},
		
		];
		/*
		 public const byte CTRL_TEMP = 1;
        public const byte CTRL_HUM = 2;
        public const byte CTRL_DEHUM = 4;
        public const byte CTRL_OZ = 8;
        public const byte CTRL_VOC = 0x10;
        public const byte CTRL_CO2 = 0x20;
        public const byte CTRL_CO = 0x40;
		*/
		var cc=cf.ctrl;
		 var ctrl_left={
			 rows:[
			 		{view:"checkbox",name:"ccb1", label:"Enable temperature" ,labelWidth:220,value:getBool(cc,1)},
					{ view: "checkbox", name: "ccb2", label: "Enable humidification", labelWidth: 220, value: getBool(cc, 2) },
					{ view: "checkbox", name: "ccb3", label: "Enable dehumidification", labelWidth: 220, value: getBool(cc, 4) },
					{ view: "checkbox", name: "ccb4", label: "Enable Ozone", labelWidth: 220, value: getBool(cc, 8) },
					{ view: "checkbox", name: "ccb5", label: "Enable VOC", labelWidth: 220, value: getBool(cc, 16) },
					{ view: "checkbox", name: "ccb6", label: "Enable CO2", labelWidth: 220, value: getBool(cc, 32) },
					{ view: "checkbox", name: "ccb7", label: "Enable Ext1", labelWidth: 220, value: getBool(cc, 64) },
					{ view: "checkbox", name: "ccb8", label: "Enable Ext2", labelWidth: 220, value: getBool(cc, 128) },
			 ]
		 };
		 
		 var ctrl_right={
			 rows:[
			 	 {margin:30,},
				 { template:"Assign dry contact",width:300, type:"section"},
				 { view:"combo",name:"cb_0",value:get_dry(cf, 0),width:200, options:ctrl_o  },
				 { view:"combo",name:"cb_1",value:get_dry(cf, 1),width:200, options:ctrl_o   },
				 { view:"combo",name:"cb_2",value:get_dry(cf, 2),width:200, options:ctrl_o   },
				 { view:"combo",name:"cb_3",value:get_dry(cf, 3),width:200,options:ctrl_o   },
				 { view:"combo",name:"cb_4",value:get_dry(cf, 4),width:200, options:ctrl_o  },	
			 ]
		 };
		
		var frm6={
		    view: "form", id: getFI(6), padding: 8,
		    elements: [
                {
                    rows: [
                       { cols: [ctrl_left, { view: "spacer", }, ctrl_right] },
                      // ctrl_air_ui
                    ]
                }
		    ],//elements: [
			
			//elementsConfig: { labelWidth: 320, labelAlign: "left" }
		};
		//====================================OPtions========================
	    var intr=4;
		switch(dc.interval)
		{
			case 12:
			intr=1;
			break;
			case 24:
			intr=2;
			break;
			case 48:
			intr=3;
			break;
		}
	    var d_notif_ui={
								view:"fieldset",label:"Notification",
								body:
								{ rows:[
									{ view:"checkbox",name:"n_en", labelRight:"Enable notifications for this device" ,value:dc.notif_enabled},
									{ view:"radio", name:"intr",value:intr,label:"Send email notification every",labelWidth: 220,/*vertical:true,*/ 
										options:[
											{ value:"12 hours", id:1 },
											{ value:"24 hours", id:2 },
											{ value:"48 hours", id:3 },
											{ value:"One week", id:4 }
										] , width:700, align:"left"},
									]//rows
								}//body
						   };
						 
	    
						   
	     var mo_left={    
			 rows:[
			 		/*{
							view:"combo",
							id: "mo_cb_id",
                            name:"cb_ms",
							value:sv+1, 
							label:"Select sensor",
							options:[{id:1,value:"None"},{id:2,value:"VOC"},{id:3,value:"CO2"}],
							on:{
								//'onChange': function(id){ alert("you have clicked on an item with id="+id);ths.getInsCats(id) }
							},
							labelPosition:"top",
						}, */
						{view:"checkbox",name:"cor_en", label:"Enable corrections" ,labelWidth:160,value:false,click: cb_enable_corr_click},
						{
							id:"cor_psw_id",
							rows:[
								{ view:"text",id:"cor_pws_txt",name:"cor_pws",width:200,labelWidth:120,type: "password",  label:"Password" },
								{ view: "button",  label: "Confirm",click: corr_password_enter },
							]
						}
			 ]
	     };
	     
		 var mo_right={
			 id:"corr_id",
			 rows:[
			 	 {margin:30,},
				// { template:"Assign dry contact with value",width:350, type:"section"},
				 { view:"text",name:"cor_t",width:200,labelWidth:120, value:toStr(cf.corr.temp), label:"T" },
				 { view:"text",name:"cor_h",width:200, labelWidth:120,value:toStr(cf.corr.hum), label:"RH" },
				 { view: "text", name: "cor_voc", width: 200, labelWidth: 120, value: toStr(cf.corr.VOC), label: "VOC" },
				 { view: "text", name: "cor_co2", width: 200, labelWidth: 120, value: toStr(cf.corr.CO2), label: "CO2" },
				 { view: "text", name: "cor_ext1", width: 200, labelWidth: 120, value: toStr(cf.corr.Oz), label: "Ext1" },
				  { view: "text", name: "cor_ext2", width: 200, labelWidth: 120, value: toStr(cf.corr.CO), label: "Ext2" },
			 ]
		 };
	     var d_multiopt_ui={
								view:"fieldset",label:"Corrections",
								body:{ cols:[mo_left,{ view:"spacer", },mo_right]}//body
						   };
		
		var frm7 = {
		    view: "form", id:getFI(7) , padding: 18,elements: [{rows:[d_notif_ui,{ view:"spacer", },d_multiopt_ui]}],
			rules:{
				$all:mkrwa.rules.isNotEmpty
			},
			//elementsConfig: { labelWidth: 120, inputWidth:200,labelAlign: "left" }
		};
		
		var ui_d_conf_pu_bd = {
			   
		   		view:"tabview",
				height:500,
				width:800,
				animate:false,
				
				tabbar:{
					close:false
				},
				cells:[
					{
						header:"T-RH",
						body:frm3
					},
					{
						header:"VOC-CO2",
						body:frm4
					},
					{
						header:"Ext1-Ext2",
						body:frm5
					},
					{
						header:"Controls",
						body:frm6
					},
					{
						header:"Options",
						body:frm7
					}
				]
			};
		
		
	

//windows
		var dev_config_pu = {
			view: "window", modal: true, id: "dev_conf_win", 
						height:500,
						width:800,
						left:100, top:100,
						move:true,
						//resize: true,
						css: "bradius",
			//head: { view:"toolbar", cols:[ { view:"label", template:"Add new order"} ] },
			head: {view:"toolbar", cols:[
								{view:"label",id:"lbl_title_id", label: ""  },
								{ view:"button",width:160, icon:"save",type:"iconButton",label:"Save and Exit",click:saveDevConf},
								{ view:"icon", icon:"times-circle",	click:"$$('dev_conf_win').close();"}
								] 
				  },
			body: ui_d_conf_pu_bd
		};
		
		mkrwa.ui(dev_config_pu).show();
			//woMap.setPos(44.44,-79.66,12);
			//$$("dev_conf_win")
		//var xx=$$("dev_conf_win");
			//xx.define("width", 1000);
			//xx.define("height", 1000);
		//xx.resize();
		$$("lbl_title_id").setHTML("<div><span>Configuration for </span><span style='color:yellow'>" + a.Desc + " ( " + a.DeviceID + " ) " + "</span></div>");
		if (!_cb_2) {
		    $$("ctrl_air_ui_id").hide();
		    $$("cb_ext21_id").hide();
		}
		if(_cb_rad)
		{
			$$("d_len_id").hide();
			$$("d_hei_id").hide();
		}
		$$("corr_id").hide();
		$$("cor_psw_id").hide();
		
		air_flow_opt_chgd(cf.aux_conf);
		ext1_opt_chgd(/*cf.aux_conf*/1);
}
function _drc_(v) {
    v -= 1;
    if (v < 0) v = 0;
    if (v > 10) v = 10;
    return v;
}
function cor_lim(n)
{
    n = n * 1;
	if(n>127) n=127; else if(n<-127) n=-127;
	return n;
}
function saveDevConf()
{
    var a = getAsset(sel_d.did);
    var dc = a.dconf, cf = a.dconf.config;
    if (!dc || !cf)
        return;
	//device config must exist !!
	var f3 = getFV(3), f4 = getFV(4), f5 = getFV(5), f6 = getFV(6),f7 = getFV(7);
    //1. for T-RH form
	cf.lcdu.TempHiLimit = f3.thu, cf.lcdu.TempLoLimit = f3.thl, cf.lcdu.CoolSetPoint = f3.tcp, cf.lcdu.HeatSetPoint = f3.thp, cf.lcdu.HumHiLimit = f3.hhu, cf.lcdu.HumLoLimit = f3.hhl, cf.lcdu.DehumSetPoint = f3.dsp, cf.lcdu.HumSetPoint = f3.hsp;
    //2. CO2-VOC form
	cf.lcdu.rVOC = f4.vor, cf.lcdu.VOC_Hi = f4.vou, cf.lcdu.VOC_Lo = f4.vol, cf.lcdu.rCO2 = f4.cor, cf.lcdu.CO2_Hi = f4.cou, cf.lcdu.CO2_Lo = f4.col;
    
	cf.aux1_range = f5.ext1_range;
	cf.aux2_range = f5.ext2_range;
	cf.ip_flag &= 0xf87f;       // clear bits for extenal flags
	if (f5.cb_ext1)
	    cf.ip_flag |= 0x80;  // use Ozone
	if (f5.cb_ext2)
	    cf.ip_flag |= 0x100; // use air flow sensor
	if (f5.cb_ext21)
	    cf.ip_flag |= 0x200;  // use metrics
	cf.aux_width = f5.d_len;
	if(f5.cb_rad)
	  cf.ip_flag |= 0x400;  // use radius to calculate duct cross section
	cf.aux_height = f5.d_hei;
    cf.aux_conf=parseInt(f5.combo_aif);    // index  of selected air flow sensor
	if(isNaN(cf.aux_conf))
		cf.aux_conf=0;
	// Controls form
	cf.ctrl = f6.ccb1 | (f6.ccb2 << 1) | (f6.ccb3 << 2) | (f6.ccb4 << 3) | (f6.ccb5 << 4) | (f6.ccb6 << 5);
	cf.json_t_outs[0] = _drc_(f6.cb_0), cf.json_t_outs[1] = _drc_(f6.cb_1), cf.json_t_outs[2] = _drc_(f6.cb_2), cf.json_t_outs[3] = _drc_(f6.cb_3), cf.json_t_outs[4] = _drc_(f6.cb_4);

    // Options form
	// DO NOT USE aux
	dc.notif_enabled = f7.n_en;
	dc.interval = f7.intr;
	
    cf.corr.temp=cor_lim(f7.cor_t);
	cf.corr.hum=cor_lim(f7.cor_h);
	cf.corr.VOC=cor_lim(f7.cor_voc);
	cf.corr.CO2=cor_lim(f7.cor_co2);
	cf.corr.Oz=cor_lim(f7.cor_ext1);
	cf.corr.CO=cor_lim(f7.cor_ext2);
   
	if(f3.t_u=='1')
	{
		cf.ip_flag|=8;
	}else cf.ip_flag&=0xfff7;


    //ext1 sensor info
	var ss = getExt1SensorVals();
	if (ss)
	{
	    cf.ext1_sensor.sensor_name = ss.sensor_name;
	    cf.ext1_sensor.sensor_unit = ss.sensor_unit;
	    cf.ext1_sensor.min_value = ss.min_value;
	    cf.ext1_sensor.max_value = ss.max_value;
	}
	
	//send config to the server
	var	v = gPrms(51) + "&did="+sel_d.did+"&json=" + JSON.stringify(dc);
	
	AJX.post(myurl, v, function (d) {
			if(d=="No")		
			{
				mkrwa.alert({
					//title:"Custom title",
					ok:"OK",
					type:"alert-warning",
					text:"Could not save config. Try again later",
					//callback:function(){...}
				});
			}
		});
		
	$$('dev_conf_win').close();
}

function cb_click()  // use as Ozone
{
	var x=$$("ext1_range_id"),f5=getFV(5);
	if(f5.cb_ext1){
	//x.define("label", "Range ( ppb )");
	x.define({value:"120",label:"Range ( ppb )"});
	
	}else x.define({value:"100",label:"Range ( % )"});
	x.refresh();
}
function cb_click1()  // use as Air Flow
{
    var x = $$("ext2_range_id"), f5 = getFV(5), z = $$("ctrl_air_ui_id"), w = $$("d_len_id"), h = $$("d_hei_id"), y = $$("cb_ext21_id");
    var u = "( mm )", l1 = "Duct length ", l2 = "Duct height ";
	var val=f5.ext2_range;
	if (f5.cb_ext2) {
	    z.show();
	    y.show();
	    if (f5.cb_ext21) {
	        x.define({ value: val, label: "Range ( cmh )" });
	        w.define({ label: l1+u });
	        h.define({ label: l2+u });
	    }
	    else {
	        x.define({ value: val, label: "Range ( cfm )" });
	        u = "( inch )";
	        w.define({ label: l1 + u });
	        h.define({ label: l2 + u });
	    }
	    w.refresh();
	    h.refresh();
	}
	else {
	    x.define({ value: "100", label: "Range ( % )" });
	    z.hide();
	    y.hide();
	}
	x.refresh();
}
function cb_click2()  // use Metric
{
	//d_len,d_hei,d_rad_val
    var x = $$("ext2_range_id"), f5 = getFV(5), w = $$("d_len_id"), h = $$("d_hei_id"),rad = $$("d_rad_id");
    var u = "( mm )", l1 = "Duct length ", l2 = "Duct height ",lr="Radius ";
	var val=f5.ext2_range;
	var wv=f5.d_len,hv=f5.d_hei,rv=f5.d_rad_val;
	if (f5.cb_ext21) {
		// convert from inch to mm
		wv*=25.4;hv*=25.4;rv*=25.4;
	    x.define({ value: val, label: "Range ( cmh )" });
	   
	}
	else {
		wv/=25.4; hv/=25.4; rv/=25.4;
	    x.define({ value: val, label: "Range ( cfm )" });
	    u = "( inch )";
	   
	}
	 w.define({ label: l1 + u ,value:wv.toFixed()});
	 h.define({ label: l2 + u ,value:hv.toFixed()});
	 rad.define({ label: lr + u ,value:rv.toFixed()});
	w.refresh();
	h.refresh();
	rad.refresh();
	x.refresh();
	
	update_airf_range();
}
function cb_rad_use_click()
{
	var f5 = getFV(5);//, w = $$("d_len_id"), h = $$("d_hei_id"),rad = $$("d_rad_id");
	var z=$$("view_rect_id");
	if(f5.cb_rad)
	{
		z.hide();
		//h.hide();
	}else{
		z.show();
		//w.show();
		
	}
	//w.refresh();
	//h.refresh();
}
//ESF-35-2 Air Flow  -> 0-16 m/sec (3150 ft/min)
var AIRF_I=3150,AIRF_M=16; // air flow sensor imperial / metric
function update_airf_range()
{
	var a = getAsset(sel_d.did);
    var dc = a.dconf, cf = a.dconf.config;
    if (!dc || !cf)
        return;
	var x = $$("ext2_range_id"),f5 = getFV(5);
	var w=f5.d_len,h=f5.d_hei,range=0,f=f5.cb_ext21,p=3.14159;
	if(f5.cb_rad)
	{
		w=f5.d_rad_val;
		if (!f)
		{
			w=w*1.0 /12.0;   //  in inches
			range=AIRF_I*p*w*w;
		}
		else
		{
			w=w*1.0/ 1000.0;  // in millimeters
			range=AIRF_M*3600*p*w*w;
		}
		
	}
	else{
		if (!f)
		{
			range = AIRF_I * h*w / (12 * 12);   // height and width in inches
		}
		else
		{
			range = AIRF_M * h*w *3600 / (1000 * 1000);  // height and width in millimeters
		}
	}
	 x.define({ value: range.toFixed()});
	 x.refresh();
}

function cb_enable_corr_click()
{
	var f7 = getFV(7),x=$$("cor_pws_txt");
	if(f7.cor_en)
		$$("cor_psw_id").show();
	else{
		$$("cor_psw_id").hide();
		$$("corr_id").hide();
		 x.define({ value:""});
		 x.refresh();
	}
}
function corr_password_enter()
{
	var f7 = getFV(7);
	if(f7.cor_en)
	{
		var s=f7.cor_pws;
		if(!isEmpty(s)&&s=="11")
		{
			$$("corr_id").show();
		}else
			$$("corr_id").hide();
	}
}
function air_flow_opt_chgd(id)
{
	//alert("id="+id);
	switch(id)
	{
		case 1:
		AIRF_I=3150,AIRF_M=16;
		break;
		case 2:
		AIRF_I=1575,AIRF_M=8;
		break;
	}
}

//----------------------------------------------------------------------
function ext1_opt_chgd(id) {
    var x=$$("ext1_range_id"),z=getExt1SensorVals();
	if(x&&z)
	{
	 x.define({ value: parseInt(z.max_value).toFixed()});
	 x.refresh();
	}
}

function add_new_ext1_sensor() {
	 var a = getAsset(sel_d.did);
    var dc = a.dconf, cf = a.dconf.config;
    if (!dc || !cf)
        return;
    var ext1_sens_ui = {

        view: "fieldset", label: "Ext1 new sensor",

        body: {
            rows: [
				// { template:"***",width:350, type:"section"},
				{ margin: 30, },
				 { view: "text", name: "new_sens_name", width: 350, value: "", label: "Sensor name" },
				 { view: "text", name: "new_sens_unit", width: 350, inputWidth: 210, value: "", label: "Sensor unit" },
				 { view: "text", name: "new_sens_min_val", width: 350,inputWidth: 210,  value:"", label: "Min value" },
                 { view: "text", name: "new_sens_max_val", width: 350,inputWidth: 210,  value:"", label: "Max value" },

            ]
        }//body

    };


    var ext1_new_sens_frm = {
        view: "form", id: "ext1_new_sens_frm_id", padding: 18, elements: [ext1_sens_ui],
        elementsConfig: { labelWidth: 130, labelAlign: "left" }
    };

    var ext1_sensor_pu = {
        view: "window", modal: true, id: "ext1_sensor_win",
        height: 300,
        width: 400,
        left: 100, top: 100,
        move: true,
        //resize: true,
        css: "bradius",
        //head: { view:"toolbar", cols:[ { view:"label", template:"Add new order"} ] },
        head: {
            view: "toolbar", cols: [
                         { view: "label", id: "lbl_title_id", label: "" },
                         { view: "button", width: 160, icon: "save", type: "iconButton", label: "Save and Exit", click: saveNewExt1Sensor },
                         { view: "icon", icon: "times-circle", click: "$$('ext1_sensor_win').close();" }
            ]
        },
        body: ext1_new_sens_frm
    };

    mkrwa.ui(ext1_sensor_pu).show();
}
function saveNewExt1Sensor() {
    var a = getAsset(sel_d.did);
    var dc = a.dconf, cf = a.dconf.config;
    if (!dc || !cf)
        return;

    //send config to the server
    var f = $$("ext1_new_sens_frm_id").getValues();
    var ss = { sensor_name: f.new_sens_name, sensor_unit: f.new_sens_unit, min_value: f.new_sens_min_val, max_value: f.new_sens_max_val };    //EXT1_SENSOR_INFO
    
    var v = gPrms(65) + "&json=" + JSON.stringify(ss);
    AJX.post(myurl, v, function (d) {
        if (d == "No") {
            mkrwa.alert({
                //title:"Custom title",
                ok: "OK",
                type: "alert-warning",
                text: "Could not save ext1 sensor. Try again later",
                //callback:function(){...}
            });
        }else if (d == "OK") 
        {
            if (!gO.ext1_sensors)
                gO.ext1_sensors = [];
            gO.ext1_sensors.push(ss);

            var xx = $$("combo_ext1_id");
            var zz = xx.getPopup().getList();
			var clen = zz.count();
            var q = { id: clen+1, value: ss.sensor_name};
            zz.add(q);
            zz.refresh();  // refresh ext1 sensors list
        }
    });

    $$('ext1_sensor_win').close();
}
function getExt1SensorVals()
{
    var n = $$("combo_ext1_id").getText();
    if(!isEmpty(n))
    {
        var Z = gO.ext1_sensors;
        if (Z && Z.length > 0) {
            for (var i = 0; i < Z.length; i++) {
                if (Z[i].sensor_name == n) 
                    return Z[i];
            }
        }
    }
    return null;
}