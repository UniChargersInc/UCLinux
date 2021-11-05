//=====Resources==============
var gRes=null; // list of resources ( drivers)
function g_r_list(cb) {
if(gRes!=null&&gRes.length>0)
{ 
	if(cb)cb();
	return;
}
	AJX.post(URL, gPrms(21), function (d) {
		var o = JSON.parse(d);
		if (o != null&&o.resource_list!=null) {
			gRes=o.resource_list;
			if(cb)cb();
		}
	});
		
}
function makeRes(id) {
    var ar = [];
    for (var i = 0; i < gRes.length; i++) {
        var o = gRes[i], q, f=false;
        if (isNDef(id)) {
            if (i == 0) f=true
        }
        else
            if (o.ID == id) f = true;
        q = { id: o.ID, name: o.DriverName,photo:o.photo, selected: f};
        ar.push(q);
    }
    return ar;
}
function getResInd(id) {
    if (gRes != null) {
        for (var i = 0; i < gRes.length; i++)
            if (gRes[i].ID == id)
                return i;
    }
    return -1;
}
function getResName(id)
{
    var i = getResInd(id);
    return (i != -1 ? gRes[i].DriverName : "");
}
function getRes(id)
{
	var i = getResInd(id);
	return (i != -1 ? gRes[i] : null);
}
function updateRLoc(o) //update resources from heartbeat o=RttHBEvent
{
			
	var rid,vv,i;
	try{
		x=o.esn.split('-');   // esn=MOBID-1032 -> rid=1032
		rid=x[1];
	}catch(e){}
	if(isNDef(rid))
		return;
	 i=getResInd(rid);
	 vv=i!= -1 ? gRes[i] :null;
	 if(vv)
	 {
		 vv.Loc=new gloc(o.lat,o.lon);
	 }
	
}

function mob_w_fn(o)
{
    var ph=o.photo;
    if(isEmpty(ph))ph="blank.png";
    var cf="<img height='32' width='32' src='"+cPath+"resImg/"+ph+"' style='float:left;'>";
    var h="<span style='margin-left:10px'>"+o.name+"</span>";
    return "<div>"+cf+h+"</div>";
}


//========================= HOS Reports=============================================
function RHOS(id)
{
	var ths=this;
	var tv,_id;
	this.init=function()
	{
		_id = "tb_" + id;
		
	
		var ts=new Date();
		var ts0=new Date();
		ts0.setTime( ts0.getTime() - 1/*days*/ * 86400000 );
		
		var hos_item_selector = {
			width: 360,
			type:"clean", 
			rows:[

				{
					id:"hos_item_id",
					view:"list",
					template:"html->rhos_tmplt",
				
					select:true,
					on:{ "onafterselect":ths.item_selected },
					type:{ height: 84 },
					ready:function(){  //select USA
						//this.select(6);
					}
				},
				{
					view:"form", elements:[
						{ height: 10},
						{
							view:"combo",
							id:"mob_w_id", 
							label:"",
							options:{
							    body: {
							        data: [],
							        template: mob_w_fn
							    }
							},
							
							placeholder:"Select mobile worker"
						}, 
						
						{view:"datepicker", label:"From", value:ts0, format:"%d  %M %Y"},
						{view:"datepicker", id:"datepicker1", label:"From", value:new Date(), format:"%d  %M %Y", hidden:true},
						{view:"datepicker", label:"To", value:ts, format:"%d  %M %Y"},
						{view:"datepicker", id:"datepicker2", label:"To", value:new Date(), format:"%d  %M %Y", hidden:true},
						{view:"checkbox", id:"flexible", value:0, label: "Last 14 days"},
						
						{ height: 10},
						{view:"button", type:"form", value:"Get Hos Logs", inputWidth:140, align: "center",click:ths.getHosEvents}, {}
	
					],
					elementsConfig:{ 
						labelWidth:100, labelAlign:"left"
					}
					
				},
			]
		};

        var r_container={
			view:"iframe", 
			id:"rhos_frame_id", 
			src:""
		};
		
		var ui_rhos = {
	        id:_id,
			rows:[
				
				{
					type: "space",
					rows:[
					{autoheight:true, type: "wide", cols:[hos_item_selector, r_container]}]
				}
			]
		};
		
		tv = {
			header: "HOS Reports",
			width: 160,
			close: true,
			body: ui_rhos
		};
		
		mTv.addView(tv);
		mTb.setValue(_id);        // change the visible tab !!!!!
		
				
		g_r_list(function() {     // get resources list -> Global function
						    var vv=makeRes();
							var xx=$$("mob_w_id");
							var zz=xx.getPopup().getList(); 
							for (var i = 0; i < vv.length; i++) {
							  e = vv[i];
							  zz.add(e);
							}
							
						});
	}//init
	this.getHosEvents = function () {
        var rid = $$("mob_w_id").getValue();
		ths.rid=rid;
		if (rid != "") {
            var params = gPrms(20)+ "&rID=" + rid+"&from="+ths.from+"&to="+ths.to;
			
            AJX.post(URL , params, function (d) {
                //alert(loader.xmlDoc.responseText);
                var obj = JSON.parse(d);
                if (obj != null) {
                    var gr=$$("hos_item_id");
					gr.clearAll();
					
                    for (var i = 0; i < obj.length; i++) {
                        var o = obj[i];
						
                        var q = {id:(i + 1),dt:o.dt,count: o.count};
                        gr.add(q,0);
						
                    }
					
                }

            });

        }
        else {
            alert("Please, Select the driver")
        }
    }
	this.item_selected =function(id) {
		var o = $$("hos_item_id").getItem(id);
		ths.getPdf(ths.rid,o.dt);
	}
	this.getPdf=function(rid,dt)
	{
	
		var u=_URL+"rpdf.ashx";
			
			var ifr=$$("rhos_frame_id").getIframe();
			ifr.src=u+"?req=11&rid=" + rid  + "&dt=" + dt+ "&rn=" + g_rand();
		
	}
	this.dispose=function()
	{
	}
	this.init(id);
	return this;
}