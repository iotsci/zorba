import module namespace geo = "http://expath.org/ns/geo";
declare namespace gml="http://www.opengis.net/gml";

geo:centroid(<gml:Curve><gml:segments>
                <gml:LineStringSegment interpolation="linear"><gml:posList>0 0 1 1 0 0 1 1 2 2</gml:posList></gml:LineStringSegment>
                <gml:LineStringSegment><gml:posList> 1 1 2 2 4 -40</gml:posList></gml:LineStringSegment>
              </gml:segments></gml:Curve>
              )