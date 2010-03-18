  import module namespace xqddf = "http://www.zorba-xquery.com/modules/xqddf";

  import schema namespace news-schemas = "http://www.news.org/schemas" at "news-schema.xsd";

  import module namespace news-data = "http://www.news.org/data" at "index_does_not_exist.xqlib";

  xqddf:create-collection($news-data:employees);

  xqddf:probe-index-point($news-data:CityEmp, "Paris");