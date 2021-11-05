REM m_load.js = 9185

::perl -i.bk -pe 's/[^[:ascii:]]//g;' m_load.js

sed -i "s/[\d128-\d255]//g"  tms-0.3.js

::perl -pi -e 's/[^[:ascii:]]//g' m_load.js

::minify m_load.js udorx_ws_comm.js > all_pbf.min.js