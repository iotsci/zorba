(:
 : Copyright 2006-2016 zorba.io
 :
 : Licensed under the Apache License, Version 2.0 (the "License");
 : you may not use this file except in compliance with the License.
 : You may obtain a copy of the License at
 :
 : http://www.apache.org/licenses/LICENSE-2.0
 :
 : Unless required by applicable law or agreed to in writing, software
 : distributed under the License is distributed on an "AS IS" BASIS,
 : WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 : See the License for the specific language governing permissions and
 : limitations under the License.
:)

declare namespace ann = "http://zorba.io/annotations";
declare namespace extern="urn:extern";
declare %ann:nondeterministic function extern:nondeterm() external;

let $r := extern:nondeterm( )
return
   if ($r > 0 and $r < 10) then "Yes" else "No" ,

extern:nondeterm()
