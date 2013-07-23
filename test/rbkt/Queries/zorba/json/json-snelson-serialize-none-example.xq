import module namespace jx = "http://zorba.io/modules/json-xml";

let $xml :=
  <json type="object">
    <pair name="firstName" type="string">John</pair>
    <pair name="lastName" type="string">Smith</pair>
    <pair name="address" type="object">
      <pair name="streetAddress" type="string">21 2nd Street</pair>
      <pair name="city" type="string">New York</pair>
      <pair name="state" type="string">NY</pair>
      <pair name="postalCode" type="number">10021</pair>
    </pair>
    <pair name="phoneNumbers" type="array">
      <item type="string">212 732-1234</item>
      <item type="string">646 123-4567</item>
    </pair>
  </json>
return jx:xml-to-json-string( $xml )

(: vim:set et sw=2 ts=2: :)
