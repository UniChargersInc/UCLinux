function parseConf(hex,ind) {
    var buf = parseHexString(hex);
    var cf = {};
    cf.flags = aFrame[ind++];
    cf.version = aFrame[ind++];
    // fw_version = cf.version;
    cf.ctrl = aFrame[ind++];

    cf.lcdu = {};
    parseLcdu(cf.lcdu);

    parseCorr( cf);
  
    parseDeviceInfo( cf);

    parseOuts( cf);
    try
    {
        parseAux( cf);
        parseStaticIPs( cf);
    }
    catch (err) {alert(err) }
}

//------------------Helpers---------------------------------
function parseLcdu(cf)
{
cf.lcdu.TempHiLimit = BitConverter.ToUInt16(aFrame, startIndex);
startIndex += 2;
cf.lcdu.TempLoLimit = BitConverter.ToUInt16(aFrame, startIndex);
startIndex += 2;

cf.lcdu.HumHiLimit = BitConverter.ToUInt16(aFrame, startIndex);
startIndex += 2;
cf.lcdu.HumLoLimit = BitConverter.ToUInt16(aFrame, startIndex);
startIndex += 2;

cf.lcdu.Tref = BitConverter.ToUInt16(aFrame, startIndex);
startIndex += 2;
cf.lcdu.Href = BitConverter.ToUInt16(aFrame, startIndex);
startIndex += 2;


cf.lcdu.CoolSetPoint = BitConverter.ToUInt16(aFrame, startIndex);
startIndex += 2;
cf.lcdu.HeatSetPoint = BitConverter.ToUInt16(aFrame, startIndex);
startIndex += 2;

cf.lcdu.HumSetPoint = BitConverter.ToUInt16(aFrame, startIndex);
startIndex += 2;
cf.lcdu.DehumSetPoint = BitConverter.ToUInt16(aFrame, startIndex);
startIndex += 2;

cf.lcdu.TtolP = BitConverter.ToUInt16(aFrame, startIndex);
startIndex += 2;
cf.lcdu.TtolM = BitConverter.ToUInt16(aFrame, startIndex);
startIndex += 2;

cf.lcdu.HtolP = BitConverter.ToUInt16(aFrame, startIndex);
startIndex += 2;
cf.lcdu.HtolM = BitConverter.ToUInt16(aFrame, startIndex);
startIndex += 2;

cf.lcdu.rCO2 = BitConverter.ToUInt16(aFrame, startIndex);
startIndex += 2;
cf.lcdu.rOz = BitConverter.ToUInt16(aFrame, startIndex);
startIndex += 2;
cf.lcdu.rVOC = BitConverter.ToUInt16(aFrame, startIndex);
startIndex += 2;
cf.lcdu.rCO = BitConverter.ToUInt16(aFrame, startIndex);
startIndex += 2;

cf.lcdu.CO2_Hi = BitConverter.ToUInt16(aFrame, startIndex);
startIndex += 2;
cf.lcdu.CO2_Lo = BitConverter.ToUInt16(aFrame, startIndex);
startIndex += 2;

cf.lcdu.VOC_Hi = BitConverter.ToUInt16(aFrame, startIndex);
startIndex += 2;
cf.lcdu.VOC_Lo = BitConverter.ToUInt16(aFrame, startIndex);
startIndex += 2;

cf.lcdu.Oz_Hi = BitConverter.ToUInt16(aFrame, startIndex);
startIndex += 2;
cf.lcdu.Oz_Lo = BitConverter.ToUInt16(aFrame, startIndex);
startIndex += 2;

cf.lcdu.CO_Hi = BitConverter.ToUInt16(aFrame, startIndex);
startIndex += 2;
cf.lcdu.CO_Lo = BitConverter.ToUInt16(aFrame, startIndex);
startIndex += 2;

}