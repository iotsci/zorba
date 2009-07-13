/*
 * Copyright 2006-2008 The FLWOR Foundation.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef ZORBA_DURATIONSDATESTIMES_H
#define ZORBA_DURATIONSDATESTIMES_H

#include "common/shared_types.h"
#include "runtime/base/binarybase.h" // TODO remove after iterator refactoring
#include "runtime/base/unarybase.h" // TODO remove after iterator refactoring
#include "runtime/base/narybase.h"
#include "zorbatypes/datetime.h"


namespace zorba
{

class FnDateTimeConstructorIterator : public BinaryBaseIterator<FnDateTimeConstructorIterator,
                                                                PlanIteratorState>
{
public:
  FnDateTimeConstructorIterator(short sctx,  const QueryLoc& loc,  PlanIter_t& arg0,  PlanIter_t& arg1 )
    :
    BinaryBaseIterator<FnDateTimeConstructorIterator, PlanIteratorState>(sctx, loc, arg0, arg1){}

  virtual ~FnDateTimeConstructorIterator() {};

public:
  bool nextImpl(store::Item_t& result, PlanState& planState) const;
  virtual void accept(PlanIterVisitor&) const;
};
/*
10.1 Duration, Date and Time Types
10.1.1 Limits and Precision
10.2 Date/time datatype values
10.2.1 Examples
10.3 Two Totally Ordered Subtypes of Duration
10.3.1 xs:yearMonthDuration
10.3.2 xs:dayTimeDuration
10.4 Comparison Operators on Duration, Date and Time Values
10.4.1 op:yearMonthDuration-less-than
10.4.2 op:yearMonthDuration-greater-than
10.4.3 op:dayTimeDuration-less-than
10.4.4 op:dayTimeDuration-greater-than
*/

// 10.4.5 op:duration-equal
class OpDurationEqualIterator : public BinaryBaseIterator<OpDurationEqualIterator, PlanIteratorState>{
public:
  OpDurationEqualIterator( short sctx, const QueryLoc& loc,  PlanIter_t& arg0,  PlanIter_t& arg1 )
  :
  BinaryBaseIterator<OpDurationEqualIterator, PlanIteratorState>(sctx, loc, arg0, arg1){}

  virtual ~OpDurationEqualIterator() {};
  
public:
  bool nextImpl(store::Item_t& result, PlanState& planState) const;
  
  virtual void accept(PlanIterVisitor&) const;
};

/*
10.4.6 op:dateTime-equal
10.4.7 op:dateTime-less-than
10.4.8 op:dateTime-greater-than
10.4.9 op:date-equal
10.4.10 op:date-less-than
10.4.11 op:date-greater-than
10.4.12 op:time-equal
10.4.13 op:time-less-than
10.4.14 op:time-greater-than
10.4.15 op:gYearMonth-equal
10.4.16 op:gYear-equal
10.4.17 op:gMonthDay-equal
10.4.18 op:gMonth-equal
10.4.19 op:gDay-equal
*/

/*  
 * 10.5 Component Extraction Functions on Durations, Dates and Times
 * --------------------*/

//10.5.1 fn:years-from-duration
NARY_ITER(FnYearsFromDurationIterator);

// 10.5.2 fn:months-from-duration
NARY_ITER(FnMonthsFromDurationIterator);

// 10.5.3 fn:days-from-duration
NARY_ITER(FnDaysFromDurationIterator);

// 10.5.4 fn:hours-from-duration
NARY_ITER(FnHoursFromDurationIterator);

// 10.5.5 fn:minutes-from-duration
NARY_ITER(FnMinutesFromDurationIterator);

// 10.5.6 fn:seconds-from-duration
NARY_ITER(FnSecondsFromDurationIterator);

// 10.5.7 fn:year-from-dateTime
NARY_ITER(FnYearFromDatetimeIterator);

// 10.5.8 fn:month-from-dateTime
NARY_ITER(FnMonthFromDatetimeIterator);

// 10.5.9 fn:day-from-dateTime
NARY_ITER(FnDayFromDatetimeIterator);

// 10.5.10 fn:hours-from-dateTime
NARY_ITER(FnHoursFromDatetimeIterator);

// 10.5.11 fn:minutes-from-dateTime
NARY_ITER(FnMinutesFromDatetimeIterator);

// 10.5.12 fn:seconds-from-dateTime
NARY_ITER(FnSecondsFromDatetimeIterator);

// 10.5.13 fn:timezone-from-dateTime
NARY_ITER(FnTimezoneFromDatetimeIterator);

// 10.5.14 fn:year-from-date
NARY_ITER(FnYearFromDateIterator);

// 10.5.15 fn:month-from-date
NARY_ITER(FnMonthFromDateIterator);

// 10.5.16 fn:day-from-date
NARY_ITER(FnDayFromDateIterator);

// 10.5.17 fn:timezone-from-date
NARY_ITER(FnTimezoneFromDateIterator);

// 10.5.18 fn:hours-from-time
NARY_ITER(FnHoursFromTimeIterator);

// 10.5.19 fn:minutes-from-time
NARY_ITER(FnMinutesFromTimeIterator);

// 10.5.20 fn:seconds-from-time
NARY_ITER(FnSecondsFromTimeIterator);

// 10.5.21 fn:timezone-from-time
NARY_ITER(FnTimezoneFromTimeIterator);

// XQuery 1.1 DateTime formatting
// NARY_ITER(FnFormatDateTimeIterator);

class FnFormatDateTimeIterator : public NaryBaseIterator<FnFormatDateTimeIterator, PlanIteratorState >
{
private:
  DateTime::FACET_TYPE facet_type;
    
public:
  FnFormatDateTimeIterator(short sctx, const QueryLoc& loc, std::vector<PlanIter_t>& aChildren, DateTime::FACET_TYPE a_facet_type = DateTime::DATETIME_FACET)
    :
    NaryBaseIterator<FnFormatDateTimeIterator, PlanIteratorState >(sctx, loc, aChildren), facet_type(a_facet_type)
  {
  }

public:
  bool nextImpl(store::Item_t& result, PlanState& aPlanState) const;
};


  /* begin 10.6 Arithmetic Operators on Durations
  10.6 Arithmetic Operators on Durations
  10.6.1 op:add-yearMonthDurations
  10.6.2 op:subtract-yearMonthDurations
  10.6.3 op:multiply-yearMonthDuration
  10.6.4 op:divide-yearMonthDuration
  10.6.5 op:divide-yearMonthDuration-by-yearMonthDuration
  10.6.6 op:add-dayTimeDurations
  10.6.7 op:subtract-dayTimeDurations
  10.6.8 op:multiply-dayTimeDuration
  10.6.9 op:divide-dayTimeDuration
  10.6.10 op:divide-dayTimeDuration-by-dayTimeDuration
  - end 10.6 Arithmetic Operators on Durations*/

/*
10.7 Timezone Adjustment Functions on Dates and Time Values
*/

/*
10.7.1 fn:adjust-dateTime-to-timezone
10.7.2 fn:adjust-date-to-timezone
10.7.3 fn:adjust-time-to-timezone
*/
class FnAdjustToTimeZoneIterator_1 : public UnaryBaseIterator<FnAdjustToTimeZoneIterator_1,
                                                                PlanIteratorState>
{
public:
  FnAdjustToTimeZoneIterator_1(short sctx, const QueryLoc& loc, PlanIter_t& arg0)
    :
    UnaryBaseIterator<FnAdjustToTimeZoneIterator_1, PlanIteratorState>(sctx, loc, arg0){}

  virtual ~FnAdjustToTimeZoneIterator_1() {};

public:
  bool nextImpl(store::Item_t& result, PlanState& planState) const;
  virtual void accept(PlanIterVisitor&) const;
};

class FnAdjustToTimeZoneIterator_2 : public BinaryBaseIterator<FnAdjustToTimeZoneIterator_2,
                                                                PlanIteratorState>
{
public:
  FnAdjustToTimeZoneIterator_2(short sctx, const QueryLoc& loc, PlanIter_t& arg0, PlanIter_t& arg1)
  :
  BinaryBaseIterator<FnAdjustToTimeZoneIterator_2, PlanIteratorState>(sctx, loc, arg0, arg1){}

  virtual ~FnAdjustToTimeZoneIterator_2() {};

public:
  bool nextImpl(store::Item_t& result, PlanState& planState) const;
  virtual void accept(PlanIterVisitor&) const;
};



/*
10.7.2 fn:adjust-date-to-timezone
10.7.3 fn:adjust-time-to-timezone
10.8 Arithmetic Operators on Durations, Dates and Times
10.8.1 op:subtract-dateTimes
10.8.2 op:subtract-dates
10.8.3 op:subtract-times
10.8.4 op:add-yearMonthDuration-to-dateTime
10.8.5 op:add-dayTimeDuration-to-dateTime
10.8.6 op:subtract-yearMonthDuration-from-dateTime
10.8.7 op:subtract-dayTimeDuration-from-dateTime
10.8.8 op:add-yearMonthDuration-to-date
10.8.9 op:add-dayTimeDuration-to-date
10.8.10 op:subtract-yearMonthDuration-from-date
10.8.11 op:subtract-dayTimeDuration-from-date
10.8.12 op:add-dayTimeDuration-to-time
10.8.13 op:subtract-dayTimeDuration-from-time
*/

} // namespace zorba

#endif
