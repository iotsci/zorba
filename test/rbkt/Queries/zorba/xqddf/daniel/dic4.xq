import module namespace xqddf-test = "http://www.zorba-xquery.com/modules/xqddf/test" at "xqddf_prolog.xqlib";
import module namespace init = "http://www.zorba-xquery.com/modules/store/static-collections/initialization";
import module namespace manip = "http://www.zorba-xquery.com/modules/store/static-collections/manipulation";

init:create-collection($xqddf-test:white-collection);

init:activate-integrity-constraint($xqddf-test:dic1);

block{
<newline>
</newline>
},
block
{
for $i in fn:doc("auction.xml")//item
return $i/name;
},
block{
<newline> a
</newline>
},
block{
for $i in fn:doc("auction.xml")//item
return 
    block{block{$i/name;},
    block{manip:insert-nodes($xqddf-test:white-collection, (copy $copyi := $i modify () return $copyi));};};
},
block{
<newline> a
</newline>
},
block{
manip:delete-nodes(manip:collection($xqddf-test:white-collection)[1]);
},
block{
manip:collection($xqddf-test:white-collection)/name;
};
