(:~
 : This module provides functions for accessing and manipulating imap mailservers.
 :
 : @author Daniel Thomas 
 :)
module namespace imap = 'http://www.zorba-xquery.com/modules/email/imap';
import module namespace imapimpl = 'http://www.zorba-xquery.com/modules/email/imapimpl';
import schema namespace imaps = 'http://www.zorba-xquery.com/modules/email/imap';
import schema namespace email = 'http://www.zorba-xquery.com/modules/email/email';


(:~
 : Returns the status of a given mailbox.
 : The status of a mailbox contains:
 : <ul>
 :  <li>messages: the number of messages in the mailbox</li>
 :  <li>recent: the number of messages flagged as recent</li>
 :  <li>unseen: the number of messages flagged as unseen</li>
 :  <li>uidnext: the next unique identifier that will be assigned to a message</li>
 :  <li>uidvalidity: a value that, together with the uidnext value forms a 64 bit number that must be unique for the server</li>
 : </ul>
 :
 : @param $host-info describes the IMAP host, username and password.
 : @param $mailbox specifies the mailbox for which we want to have the status.
 : @return the status of the specified mailbox as statusType.
 : @error If the status can not be read for any reason.
 :
 :)
declare function imap:status($host-info as element(imaps:hostInfo), $mailbox as xs:string) as element(imaps:status) {
  let $status := imapimpl:status(validate {$host-info}, $mailbox)
  let $status-sequence := fn:tokenize($status, ',')
  return
      validate {
        <imaps:status>
          <messages>{$status-sequence[1]}</messages>
          <recent>{$status-sequence[2]}</recent>
          <unseen>{$status-sequence[3]}</unseen>
          <uidnext>{$status-sequence[4]}</uidnext>
          <uidvalidity>{$status-sequence[5]}</uidvalidity>
        </imaps:status> 
      
      }
};

(:~
 : Creates a new mailbox for the given user.
 :
 : @param $host-info describes the IMAP host, username and password.
 : @param $mailbox-name is the name for the new mailbox.
 : @return true if the mailbox was created successfully.
 : @error If it wasn't possible to create a connection to the IMAP server.
 : @error If the passed credentials were rejected by the IMAP server.
 :)
declare sequential function imap:create($host-info as element(imaps:hostInfo), $mailbox-name as xs:string)  as xs:boolean {
  imapimpl:create(validate {$host-info}, $mailbox-name) 
};

(:~
 : Deletes a mailbox for the given user.
 :
 : @param $host-info describes the IMAP host, username and password.
 : @param $mailbox-name is the name of the  mailbox to delete.
 : @return true if the mailbox was deleted successfully.
 : @error If it wasn't possible to create a connection to the IMAP server.
 : @error If the passed credentials were rejected by the IMAP server.
 : @error If there is no mailbox with the specified name.
 :)
declare sequential function imap:delete($host-info as element(imaps:hostInfo), $mailbox-name as xs:string)  as xs:boolean {
  imapimpl:delete(validate {$host-info}, $mailbox-name)
};

(:~
 : Renames a mailbox.
 :
 : @param $host-info describes the IMAP host, username and password.
 : @param $mailbox-old is the name of the mailbox we want to rename.
 : @param $mailbox-new is the new name for the mailbox.
 : @return true it the mailbox was renamed successfully.
 : @error If it wasn't possible to create a connection to the IMAP server.
 : @error If the passed credentials were rejected by the IMAP server.
 : @error If there is no mailbox with with the specified name ($mailbox-old). 
 :)
declare sequential function imap:rename($host-info as element(imaps:hostInfo), $mailbox-old as xs:string, $mailbox-new as xs:string) as xs:boolean {
  imapimpl:rename(validate {$host-info}, $mailbox-old, $mailbox-new) 
};

(:~
 : Lists IMAP folders for the specified user on the host that match the pattern. 
 :
 : @param $host-info describes the IMAP host, username and password.
 : @param $mailbox-ref is applied to pattern in an implementation dependent fashion to search for matching mailbox names. 
 : @param $pattern the pattern for mailboxes to look for (can include wildcards '*' and '%').
 : @param $only-subscribed when set true, only mailboxes are listed to which the user is subscribed.
 : @return true it the mailbox was renamed successfully.
 : @error If it wasn't possible to create a connection to the IMAP server.
 : @error If the passed credentials were rejected by the IMAP server.
 :)
declare function imap:list($host-info as element(imaps:hostInfo), $mailbox-ref as xs:string, $pattern as xs:string, $only-subscribed as xs:boolean) as element(imaps:mailbox)* {
  let $mailbox-sequence := imapimpl:list(validate {$host-info}, $mailbox-ref, $pattern, $only-subscribed)
  for $mailbox in $mailbox-sequence
  return
  validate {
    <imaps:mailbox>
      <hostName>{$host-info/imaps:hostName/text()}</hostName>
      <mailboxName>{fn:substring-after($mailbox, '}')}</mailboxName>
    </imaps:mailbox>
  }
};

(:~
 : Subscribes the user to the specified mailbox.
 : 
 : @param $host-info describes the IMAP host, username and password.
 : @param $mailbox is the mailbox the user wants to suscribe to.
 : @return true if the user was successfully subscribed to the mailbox.
 : @error If it wasn't possible to create a connection to the IMAP server.
 : @error If the passed credentials were rejected by the IMAP server.
 : @error If the specified mailbox does not exist.
 :)
declare sequential function imap:subscribe($host-info as element(imaps:hostInfo), $mailbox as xs:string) as xs:boolean {
  imapimpl:subscribe(validate {$host-info}, $mailbox)
};


(:~
 : Unsubscribes the user from the specified mailbox.
 : 
 : @param $host-info describes the IMAP host, username and password.
 : @param $mailbox is the mailbox the user wants to unsuscribe from.
 : @return true if the user was successfully unsubscribed from the mailbox.
 : @error If it wasn't possible to create a connection to the IMAP server.
 : @error If the passed credentials were rejected by the IMAP server.
 : @error If the specified mailbox does not exist.
 :)
declare sequential function imap:unsubscribe($host-info as element(imaps:hostInfo), $mailbox as xs:string) as xs:boolean {
  imapimpl:unsubscribe(validate {$host-info}, $mailbox)
};

(:~
 : Permanently deletes all messages of the given mailbox that have the \Deleted flag set.
 : 
 : @param $host-info describes the IMAP host, username and password.
 : @param $mailbox is the mailbox for which all messages that have the \Deleted flag set should be permanently deleted.
 : @return true if the expunge was successfull.
 : @error If it wasn't possible to create a connection to the IMAP server.
 : @error If the passed credentials were rejected by the IMAP server.
 : @error If the specified mailbox does not exist.
 :) 
declare sequential function imap:expunge($host-info as element(imaps:hostInfo), $mailbox as xs:string) as xs:boolean {
  imapimpl:expunge(validate {$host-info}, $mailbox)
};

(:~
 : Searches a mailbox for messages that match the given criteria.
 : The criteria should be a string as defined in the RFC3501 (IMAP4rev1).
 : A valid example would be: 'FROM zorba@gmail.com OR NOT SUBJECT Bug'. 
 : Depending on the value of $uid, the function will either return matching sequence numbers or unique identifiers.
 :
 : @param $host-info describes the IMAP host, username and password.
 : @param $mailbox is the mailbox to search.
 : @param $criteria is the searching criteria.
 : @param $uid when set true, the the function returns the sequence of Unique Identifiers corresponding to the matching mails, when set false (which is default) the corresponding sequence numbers are returned. 
 : @return Either the sequence of matching sequence numbers or the sequence of matching unique identifiers.
 : @error If it wasn't possible to create a connection to the IMAP server.
 : @error If the passed credentials were rejected by the IMAP server.
 : @error If the specified mailbox does not exist.
 : @error If the syntax of the passed criteria is incorrect.  
 :)
declare function imap:search($host-info as element(imaps:hostInfo), $mailbox as xs:string, $criteria as xs:string, $uid as xs:boolean?) as xs:long*{
  imapimpl:search(validate {$host-info}, $mailbox, $criteria, $uid)
}; 

(:~
 : Copies messages between mailboxes.
 : Depending on the value of $uid, the messages are either specified through their sequence number or through their unique id.
 : Both mailboxes must exist.
 : 
 : @param $host-info describes the IMAP host, username and password.
 : @param $mailbox-from is the mailbox in which the messages reside.
 : @param $mailbox-to is the mailbox in to which the messages should be copied.
 : @param $messages are the messages to be copied, specified either by their sequence number or their unique id.
 : @param $uid defines if the passed $message numbers should be treated as sequence numbers or unique identifiers.
 : @return true if the messages were copied successfully.
 : @error If it wasn't possible to create a connection to the IMAP server.
 : @error If the passed credentials were rejected by the IMAP server.
 : @error If any of the specified mailbox does not exist.
 : @error If any of the passed message numbers don't exist.
 :)
declare sequential function imap:copy($host-info as element(imaps:hostInfo), $mailbox-from as xs:string, $mailbox-to as xs:string, $messages as xs:long+, $uid as xs:boolean?) as xs:boolean {
  imapimpl:copy(validate {$host-info}, $mailbox-from, $mailbox-to, $messages, $uid, true());
};

(:~
 : Moves messages between mailboxes.
 : Depending on the value of $uid, the messages are either specified through their sequence number or through their unique id.
 : Both mailboxes must exist.
 : 
 : @param $host-info describes the IMAP host, username and password.
 : @param $mailbox-from is the mailbox in which the messages reside.
 : @param $mailbox-to is the mailbox in to which the messages should be moved.
 : @param $messages are the messages to be copied, specified either by their sequence number or their unique id.
 : @param $uid defines if the passed $message numbers should be treated as sequence numbers or unique identifiers.
 : @return true if the messages were moved successfully.
 : @error If it wasn't possible to create a connection to the IMAP server.
 : @error If the passed credentials were rejected by the IMAP server.
 : @error If any of the specified mailbox does not exist.
 : @error If any of the passed message numbers don't exist.
 :)
declare sequential function imap:move($host-info as element(imaps:hostInfo), $mailbox-from as xs:string, $mailbox-to as xs:string, $messages as xs:long+, $uid as xs:boolean?) as xs:boolean {
  imapimpl:copy(validate {$host-info}, $mailbox-from, $mailbox-to, $messages, $uid, false());
};

(:~
 : Fetches the envelope of a message. 
 : Please note that this function works <b>only</b> with message sequence numbers, not with unique identifier numbers. 
 :
 : @param $host-info describes the IMAP host, username and password.
 : @param $mailbox is the mailbox in which to search for the message.
 : @param $message-number is the message for which to fetch the envelope (depending on $uid either as message sequence number or unique identifier).
 : @return the envelope of the requested message.
 : @error If it wasn't possible to create a connection to the IMAP server.
 : @error If the passed credentials were rejected by the IMAP server.
 : @error If any of the specified mailbox does not exist.
 : @error If any of the passed message number does not exist.
 :) 
declare function imap:fetch-envelope($host-info as element(imaps:hostInfo), $mailbox as xs:string, $message-number as xs:long) as element() { 
    validate { imapimpl:fetch-envelope( validate { $host-info }, $mailbox , $message-number ) } 
};


(:~
 : Fetches a whole message.
 : 
 : @param $host-info describes the IMAP host, username and password.
 : @param $mailbox is the mailbox in which to search for the message.
 : @param $message-number is the message to fetch, denoted either by its sequence number or unique identifier.
 : @param $uid defines if the passed $message-number should be interpreted as sequence number (false, default) or unique identifier.
 : @return the message with the passed message number.
 : @error If it wasn't possible to create a connection to the IMAP server.
 : @error If the passed credentials were rejected by the IMAP server.
 : @error If any of the specified mailbox does not exist.
 : @error If any of the passed message number does not exist.
 :)
declare function imap:fetch-message($host-info as element(imaps:hostInfo), $mailbox as xs:string, $message-number as xs:long, $uid as xs:boolean) as element(email:Message) {
  validate { imapimpl:fetch-message(validate {$host-info}, $mailbox, $message-number, $uid) }
}; 

(:~
 : Fetches the subject for a message.
 : Please note that this function only works with message sequence numbers, not with unique identifiers.
 : Only the first 30 characters of a subject are fetched. 
 :
 : @param $host-info describes the IMAP host, username and password.
 : @param $mailbox is the mailbox for which we want to get the subject of a message.
 : @param $message-number denotes the message for which we want the subject.
 : @return the subject of the specified message.
 : @error If it wasn't possible to create a connection to the IMAP server.
 : @error If the passed credentials were rejected by the IMAP server.
 : @error If any of the specified mailbox does not exist.
 : @error If any of the passed message number does not exist.
 :)
declare function imap:fetch-subject($host-info as element(imaps:hostInfo), $mailbox as xs:string, $message-number as xs:long) as xs:string {
  imapimpl:fetch-subject(validate {$host-info}, $mailbox, $message-number)
};

(:~
 : Fetches the 'from' string of a message.
 : Please note that this function only words with message sequence numbers, not with unique identifiers.
 : Only the first 30 characters of a 'from' string are fetched.
 : 
 : @param $host-info describes the IMAP host, username and password.
 : @param $mailbox is the mailbox for which we want to get the 'from' string of a message.
 : @param $message-number denotes the message for which we want the 'from' string.
 : @return the 'from' string of the specified message.
 : @error If it wasn't possible to create a connection to the IMAP server.
 : @error If the passed credentials were rejected by the IMAP server.
 : @error If any of the specified mailbox does not exist.
 : @error If any of the passed message number does not exist.
 :)
declare function imap:fetch-from($host-info as element(imaps:hostInfo), $mailbox as xs:string, $message-number as xs:long) as xs:string
{
  imapimpl:fetch-from(validate {$host-info}, $mailbox, $message-number)
};


(:~
 : Fetches the unique identifier for a given message sequence number.
 :
 : @param $host-info describes the IMAP host, username and password.
 : @param $mailbox is the mailbox for which we want to get the unique identifier of a message sequence number.
 : @param $message-number is the message sequence number for which we want the unique identifier. 
 : @return the unique identifier of the given message sequence number.
 : @error If it wasn't possible to create a connection to the IMAP server.
 : @error If the passed credentials were rejected by the IMAP server.
 : @error If any of the specified mailbox does not exist.
 : @error If any of the passed message number does not exist.
 :)
declare function imap:fetch-uid($host-info as element(imaps:hostInfo), $mailbox as xs:string, $message-number as xs:long) as xs:long
{
  imapimpl:fetch-uid(validate {$host-info}, $mailbox, $message-number)
};

(:~
 : Fetches the message sequence number for a given unique identifier.
 :
 : @param $host-info describes the IMAP host, username and password.
 : @param $mailbox is the mailbox for which we want to get the message sequence number of an unique identifier.
 : @param $message-number is the unique identifier for which we want the message sequence number.
 : @return the message sequence number of the of the given unique identifier.
 : @error If it wasn't possible to create a connection to the IMAP server.
 : @error If the passed credentials were rejected by the IMAP server.
 : @error If any of the specified mailbox does not exist.
 : @error If any of the passed message number does not exist.
 :)
declare function imap:fetch-message-sequence-number($host-info as element(imaps:hostInfo), $mailbox as xs:string, $message-number as xs:long) as xs:long
{
  imapimpl:fetch-message-sequence-number(validate {$host-info}, $mailbox, $message-number)
};



