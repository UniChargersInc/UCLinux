function _clr(v)
{ 
	v=v&0x00ffffff;
	return "#"+("00000" + NumToHexStr(v)).substr(-6);
}
function g_clr(str)
{ 
	var hex=str.substr(1,6);
	return parseInt(hex, 16)
}
var c_f_id = ["conf_form0", "conf_form1", "conf_form2", "th_frm0", "voc_co2_frm","ext1_ext2_frm", "d_conf_frm1", "d_conf_opt_frm"];
function getFI(i) {
    return c_f_id[i];
}
function getFV(n) {
    return $$(getFI(n)).getValues();
}
function showConf(c)
{
	
    var s_unit = "°C", isFarV = 2;
    if(c.isFarenheit)
    {
        isFarV = 1;
        s_unit="°F";
    }
	    
	    var fui1 = [
			 {cols:[
				{ view:"text",name:"TH",width:400, value:c.TH, label:"Temperature High Limit ("+s_unit+")" },
				{ view:"colorpicker",name:"THColor",width:150,  value:_clr(c.THColor)/*"#ffaadd"*/ },
			]},
			{cols:[
				{ view:"text",name:"TL",width:400, value:c.TL, label:"Temperature Low Limit ("+s_unit+")" },
				{ view:"colorpicker",name:"TLColor",width:150,  value:_clr(c.TLColor) },
			]},
			{cols:[
				{ view:"text",width:400,name:"HH", value:c.HH, label:"Humidity High Limit" },
				{ view:"colorpicker",width:150,name:"HHColor",  value:_clr(c.HHColor) },
			]},
			{cols:[
				{ view:"text",width:400,name:"HL", value:c.HL, label:"Humidity Low Limit " },
				{ view:"colorpicker",width:150, name:"HLColor", value:_clr(c.HLColor) },
			]},
			 {cols:[
				{ view:"text",width:400,name:"VOCH", value:c.VOCH, label:"VOC High Limit" },
				{ view:"colorpicker",width:150, name:"VOCHColor", value:_clr(c.VOCHColor) },
			]},
			 {cols:[
				{ view:"text",width:400,name:"VOCL", value:c.VOCL, label:"VOC Low Limit" },
				{ view:"colorpicker",width:150, name:"VOCLColor", value:_clr(c.VOCLColor) },
			]},
			{cols:[
				{ view:"text",width:400,name:"CO2H", value:c.CO2H, label:"CO2 High Limit" },
				{ view:"colorpicker",width:150, name:"CO2HColor", value:_clr(c.CO2HColor)},
			]},
			{cols:[
				{ view:"text",width:400,name:"CO2L", value:c.CO2L, label:"CO2 Low Limit " },
				{ view:"colorpicker",width:150, name:"CO2LColor", value:_clr(c.CO2LColor) },
			]},
			
			{cols:[
				{ view:"text",width:400,name:"Ext1H", value:c.Ext1H, label:"Ext1 High Limit" },
				{ view:"colorpicker",width:150, name:"Ext1HColor", value:_clr(c.Ext1HColor) },
			]},
			{cols:[
				{ view:"text",width:400,name:"Ext1L", value:c.Ext1L, label:"Ext1 Low Limit " },
				{ view:"colorpicker",width:150, name:"Ext1LColor", value:_clr(c.Ext1LColor)},
			]},
			{cols:[
				{ view:"text",width:400,name:"Ext2H", value:c.Ext2H, label:"Ext2 High Limit" },
				{ view:"colorpicker",width:150, name:"Ext2HColor", value:_clr(c.Ext2HColor) },
			]},
			{cols:[
				{ view:"text",width:400,name:"Ext2L", value:c.Ext2L, label:"Ext2 Low Limit " },
				{ view:"colorpicker",width:150, name:"Ext2LColor", value:_clr(c.Ext2LColor) },
			]},
		];
	   
		var conf_form0={
		    view: "form", id: getFI(0), padding: 18, elements:
			[   /*
				{ view:"segmented",width:500, value:"one_id", label:"Temperature Unit", options:[
				{ id:"one_id", value:"Farenheit" },
				{ id:"two_id", value:"Celsius"},
				
				]},*/
				{ view:"radio", name:"t_unit",value:isFarV,label:"Temperature Unit", vertical:true, options:[
					{ value:"Farenheit", id:1 },
					{ value:"Celsius", id:2 }
				] , width:400, align:"center", vertical:true},

			], 
			
			elementsConfig: { labelWidth: 220, labelAlign: "top" }
		};

		var conf_form1={
		    view: "form", id: getFI(1), padding: 8, elements: fui1,
			rules:{
				$all:mkrwa.rules.isNotEmpty
			},

			elementsConfig: { labelWidth: 320, labelAlign: "left" }
		};
		var conf_form2 = {
		    view: "form", id: getFI(2), padding: 18,
			elements: [
			
					
						{ view: "text", name: "m_email", value: c.m_email, labelPosition: "top", label: "Send email notification to this address" },
											
					
						{ view: "text", name: "alt_email", value: c.alt_email, labelPosition: "top", label: "Alternative email address" },
						
					    { view:"radio", name:"flags",value:c.flags,label:"Email to use",labelWidth: 220, vertical:true, 
							options:[
								{ value:"Mail email", id:1 },
								{ value:"Alternative", id:2 }
							] , width:400, align:"center", vertical:true},
					    { view: "checkbox", name: "isEnabled", labelRight: "Allow notifications", value: c.isEnabled },
						{ view: "checkbox", name: "enable4AllDev", labelRight: "Allow notifications for all devices", value: c.enable4AllDev },

					
				],
			rules:{
				$all:mkrwa.rules.isNotEmpty
			},

		};
		
		var ui_config_pu_body = {
			   
		   		view:"tabview",
				height:500,
				width:700,
				animate:false,
				
				tabbar:{
					close:false
				},
				cells:[
					{
						header:"Common",
						body:conf_form0
					},
					{
						header:"Report Configuration",
						 
						//body:conf_form1
						body:{
								  view:"scrollview", 
								  id:"scrollview", 
								  scroll:"y", 
								  height: 400, 
								  width: 680, body:conf_form1
								}
					},
					{
						header:"Notifications",
						body:conf_form2
					}
				]
			};
		
		
	

//windows
		var win_config_pu = {
			view: "window", modal: true, id: "config-win", 
						height:500,
						width:700,
						left:100, top:100,
						move:true,
						//resize: true,
						css: "bradius",
			//head: { view:"toolbar", cols:[ { view:"label", template:"Add new order"} ] },
			head: {view:"toolbar", cols:[
								{view:"label", label: "System configuration" },
								{ view:"button",width:160, icon:"save",type:"iconButton",label:"Save and Exit",click:saveCmnConf},
								{ view:"icon", icon:"times-circle",	click:"$$('config-win').close();"}
								] 
				  },
			body: ui_config_pu_body
		};
		
		mkrwa.ui(win_config_pu).show();
			//woMap.setPos(44.44,-79.66,12);
			//$$("config-win")
		//var xx=$$("config-win");
			//xx.define("width", 1000);
			//xx.define("height", 1000);
		//xx.resize();
		
}
/*
public class ClientConfigurationSettings : ICallBack
{
    //====================================Report part================
    public float TH; // Temperature
    public float TL;
    public int THColor;
    public int TLColor;

    public float HH; // Humidity
    public float HL;
    public int HHColor;
    public int HLColor;

    public float VOCH; // VOC high level
    public int VOCHColor;

    public float CO2H; // CO2
    public float CO2L;
    public int CO2HColor;
    public int CO2LColor;

    public bool isFarenheit;

    //===========================Notification part=================
    public string m_email;
    public string alt_email;
    public int flags;
    public bool isEnabled;
    public bool enable4AllDev; // enable fr all devices
}
*/

function saveCmnConf()
{
   
    var f0 = getFV(0),f1=getFV(1), f2 = getFV(2);
   // var vv = f1.getValues(), zz = f2.getValues();
    var q = { };
    q.isFarenheit=f0.t_unit==1?true:false;  // get value from radio button

    q.TH=f1.TH;
    q.THColor=g_clr(f1.THColor);
    q.TL=f1.TL;
    q.TLColor=g_clr(f1.TLColor);

    q.HH=f1.HH;
    q.HHColor=g_clr(f1.HHColor);
    q.HL=f1.HL;
    q.HLColor=g_clr(f1.HLColor);

    q.VOCH=f1.VOCH;
    q.VOCHColor=g_clr(f1.VOCHColor);
	q.VOCL=f1.VOCL;
    q.VOCLColor=g_clr(f1.VOCLColor);

    q.CO2H=f1.CO2H;
    q.CO2HColor=g_clr(f1.CO2HColor);
    q.CO2L=f1.CO2L;
    q.CO2LColor=g_clr(f1.CO2LColor);
	
	q.Ext1H=f1.Ext1H;
    q.Ext1HColor=g_clr(f1.Ext1HColor);
    q.EXt1L=f1.Ext1L;
    q.Ext1LColor=g_clr(f1.Ext1LColor);
	
	q.Ext2H=f1.Ext2H;
    q.Ext2HColor=g_clr(f1.Ext2HColor);
    q.EXt2L=f1.Ext2L;
    q.Ext2LColor=g_clr(f1.Ext2LColor);

    q.m_email=f2.m_email;
    q.alt_email=f2.alt_email;
    q.flags=f2.flags;
    q.isEnabled=f2.isEnabled;
    q.enable4AllDev=f2.enable4AllDev;

    //send config to the server
	var	v = gPrms(50) + "&json=" + JSON.stringify(q);
	AJX.post(myurl, v, function (d) {
			doResp(d);
		});
	 $$('config-win').close();
}
function doResp(d)
{
	if(d=="No")		
	{
				mkrwa.alert({
					//title:"Custom title",
					ok:"OK",
					type:"alert-warning",
					text:"Could not save config. Try again later",
					//callback:function(){...}
				});
	}else{
		var o = JSON.parse(d);
		if(o!=null)
		{
			gO.conf = o;
		}
	}
}