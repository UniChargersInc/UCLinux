// DVIR Reports
function RPT(id)
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
					id:"veh_list_id",
					view:"list",
					template:"html->dvir_tmplt",
					
					select:true,
					on:{ "onafterselect":ths.item_selected },
					type:{ height: 84 },
					ready:function(){  //select USA
						//this.select(6);
					}
				},
				{
					view:"form",
					id:"rpt_form1",
					 elements:[
						{ height: 10},
						{
							view:"combo",
							id:"dev_cb_id", 
							label:"",
							options:{
							  body:{
								data: [],
								template:function(o)
								{
									 //var cf="<img height='32' width='32' src='"+cPath+"v_img/"+o.photo+"' style='float:left;'>";
									 var h="<span style='margin-left:10px'>"+o.id+" -- "+o.name+"</span>";
								     return "<div>"+h+"</div>";
																		}
							  }
							},
							on:{
								//'onChange': function(id){ /*alert("you have clicked on an item with id="+id);*/ths.getInsCats(id) }
							},

							placeholder:"Select device"
						}, 
						
						{view:"datepicker", name: "ts1_n",label:"From", value:ts0, format:"%d  %M %Y"},
						{view:"datepicker",  label:"From", value:new Date(), format:"%d  %M %Y", hidden:true},
						{view:"datepicker",name: "ts2_n", label:"To", value:ts, format:"%d  %M %Y"},
						{view:"datepicker",  label:"To", value:new Date(), format:"%d  %M %Y", hidden:true},
						//{view:"checkbox", id:"flexible", value:0, label: "Last 14 days"},
						
						{ height: 10},
						{view:"button", type:"form", value:"Generate Report", inputWidth:140, align: "center",click:ths.genPdfReport}, 
						{height: 10},
						{view:"button", type:"form", value:"Generate Excel", inputWidth:140, align: "center",click:ths.genXLReport},
	
					],
					elementsConfig:{ 
						labelWidth:100, labelAlign:"left"
					}
					
				},
			]
		};
		
	

        var r_container={
			view:"iframe", 
			id:"report_frame_id", 
			src:""
		};
		
		
		/*
		var ui_dvirs = {
	        id:_id,
			rows:[
				
				{
					type: "space",
					rows:[	{ view:"toolbar", height: 45,elements:[
						{view:"label", template: "<div style='font-size:18px;line-height:40px; margin-left:-4px;'>Webix Airlines Manager</div>"},{},
						{view:"icon", icon:"user"},
						{view:"icon", icon:"calendar"},
						{view:"icon", icon:"cog", popup:"lang"}
	
					]},
					{autoheight:true, type: "wide", cols:[hos_item_selector, r_container]}]
				}
			]
		};*/
		var ui_dvirs = {
        id: _id,
        type: "space", cols: [
			{
			    width: 400, css: "bigHeader",
			    header: "Report options", headerHeight: 45,
			    body: hos_item_selector
			},
			{ view: "resizer" },
			//r_container,
			/*{
			    css: "bigHeader",
			    header: "Categories", headerHeight: 45,
			    body: c_tree
			},*/
			{ view: "resizer" },
			//r_container
			{
			   // css: "bigHeader",
			   // header: "Pdf format", headerHeight: 45,
			    body: r_container
			}
		]
    };
		
		tv = {
			header: "Reports",
			width: 160,
			close: true,
			body: ui_dvirs
		};
		
		mTv.addView(tv);
		mTb.setValue(_id);        // change the visible tab !!!!!
		
	    //add ProgressBar to Report Container here !!!
		mkrwa.extend($$("report_frame_id"), mkrwa.ProgressBar);

		this.getDevices();
		$$('report_frame_id').attachEvent("onAfterLoad",function(){
          ths.stopProgressBar(); //alert("Loading is finished!");
		});
		
		
		
	}//init
	this.getDevices = function () {
        var o=gO,alen;
		alen=gO?o.assets.length:0;
            if (alen>0) {
				var xx=$$("dev_cb_id");
                var zz=xx.getPopup().getList(); 
                for (var i = 0; i < alen; i++) {
					
                    var a = o.assets[i];
                    var q = { id: a.DeviceID, name: a.Desc};
                    zz.add(q);
                }
            }
       
    };
	
	this.genReport = function (n) {
        var did = $$("dev_cb_id").getValue(),f1,vv,from,to;
		
		if (did != "") {
           f1 = $$("rpt_form1");vv = f1.getValues();
			from=vv.ts1_n.toISOString();
			to=vv.ts2_n.toISOString();
            var tz=vv.ts2_n.getTimezoneOffset();//The getTimezoneOffset() method returns the time difference between UTC time and local time, in minutes.
			
			if(n==0)
			  ths.getPdf(did,from,to,tz);
			  else
			    ths.getExcel(did,from,to,tz);

        }
        else {
            alert("Please, Select the device")
        }
	}
	this.genPdfReport = function () {
       ths.genReport(0);
	}
	this.genXLReport = function () {
       ths.genReport(1);
	}
	this.stopProgressBar=function()
	{
	    $$("report_frame_id").hideProgress();
	}
	this.show_progress_icon=function(delay) {
	    
	    $$("report_frame_id").showProgress({
	        delay: delay,
	        hide: true
	    });
        
	    setTimeout(function () {
	        ths.stopProgressBar();//$$("data").parse(small_film_set);
	    }, delay);
	}

   
	this.getPdf=function(did,from,to,tz)
	{
	    ths.show_progress_icon(30000);  // 10 sec
		var u=_myurl+"rpdf.ashx";
			
			var ifr=$$("report_frame_id").getIframe();
			//ifr.src=u+"?req=11&rid=" + rid  + "&dt=" + dt+ "&rn=" + g_rand();
			ifr.src=u+"?req=11&did="+did+"&from="+from+"&cid=" + lgin[0]+"&to=" + to+"&tz=" + tz+ "&rn=" + g_rand();
		
	}
	this.getExcel=function(did,from,to,tz)
	{
	    ths.show_progress_icon(30000);  // 10 sec
		var u=_myurl+"rpdf.ashx";
		u=u+"?req=12&did="+did+"&from="+from+"&cid=" + lgin[0]+"&to=" + to+"&tz=" + tz+ "&rn=" + g_rand();
		var d=new Date();
		var fname="rpt_"+did+"_" +d.toISOString()+ ".xlsx";
		/*
		//	ifr.src=u+"?req=12&did="+did+"&from="+from+"&cid=" + lgin[0]+"&to=" + to+"&tz=" + tz+ "&rn=" + g_rand();
		
                var link = document.createElement("a");
                link.href =u;// u+"?req=12&did="+did+"&from="+from+"&cid=" + lgin[0]+"&to=" + to+"&tz=" + tz+ "&rn=" + g_rand();
               // link.download = filename;
	            document.body.appendChild(link);
                link.click();
	            document.body.removeChild(link);
                link.remove();
          */
		  
		  	var xhr = new XMLHttpRequest();
			xhr.responseType = 'blob';
			xhr.onreadystatechange = function() {
				if (xhr.readyState == XMLHttpRequest.DONE) {
					//alert(xhr.responseText);
					var blob = new Blob([this.response], {type: 'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet'});
					var downloadUrl = window.URL.createObjectURL(blob);
					var a = document.createElement("a");
					a.href = downloadUrl;
					a.download = fname;
					document.body.appendChild(a);
					a.click();

				}
			}
			xhr.open('GET', u, true);
			xhr.send(null); 
		
	}
	this.dispose=function()
	{
	}
	this.init(id);
	
	
	return this;
}