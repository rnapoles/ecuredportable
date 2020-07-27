/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Author: Kelvin Tan  (kelvint at apache.org)
// JavaScript Lucene Query Validator
// Version: $Id$
// Tested: IE 6.0.2800 and Mozilla Firebird 0.7
//https://github.com/praized/lucene-query-validator
// Special characters are + - && || ! ( ) { } [ ] ^ " ~ * ? : \
// Special words are (case-sensitive) AND NOT OR

// Makes wildcard queries case-insensitive if true.
// Refer to http://www.mail-archive.com/lucene-user@jakarta.apache.org/msg00646.html
bool wildcardCaseInsensitive = true;

// Mutator method for wildcardCaseInsensitive.
// @param Should wildcard queries be case-insensitive?
void setWildcardCaseInsensitive(bool b)
{
  wildcardCaseInsensitive = b;
}



// validates a lucene query.
// @param query string
bool doCheckLuceneQueryValue(QString query)
{
  if(!query.isNull() && query.length() > 0)
  {
    query = removeEscapes(query);
    
    // check for allowed characters
    if(!checkAllowedCharacters(query)) return false;
    
    // check * is used properly
    if(!checkAsterisk(query)) return false;
    
    // check for && usage
    if(!checkAmpersands(query)) return false;
    
    // check ^ is used properly 
    if(!checkCaret(query)) return false;
    
    // check ~ is used properly
    if(!checkSquiggle(query)) return false;
    
    // check ! is used properly 
    if(!checkExclamationMark(query)) return false;
    
    // check question marks are used properly
    if(!checkQuestionMark(query)) return false;
    
    // check parentheses are used properly
    if(!checkParentheses(query)) return false;
    
    // check '+' and '-' are used properly      
    if(!checkPlusMinus(query)) return false;
    
    // check AND, OR and NOT are used properly
    if(!checkANDORNOT(query)) return false;    
    
    // check that quote marks are closed
    if(!checkQuotes(query)) return false;
    
    // check ':' is used properly
    if(!checkColon(query)) return false;
    
    if(wildcardCaseInsensitive)
    {
      if(query.indexOf("*") != -1)
      {
        int i = query.indexOf(':');
        if(i == -1)
        {
          query = query.toLower();
        }
        else // found a wildcard field search
        {
          query = query.mid(0, i) + query.mid(i).toLower();
        }
      }
    }
    return true;
  }
}

// remove the escape character and the character immediately following it
QString removeEscapes(QString query)
{
  query.replace(QRegExp("\\."),"");
}

QString checkAllowedCharacters(QString query)
{
  
  int matches = query.indexOf(QRegExp("[^a-zA-Z0-9_+\-:.()\"*?&|!{}\[\]\^~\\@#\/$%'= ]"));
  	
  //if(matches != -1 && matches.length > 0)
  if(matches != -1)
  {
    //if(alertUser) alert("Invalid search query! The allowed characters are a-z A-Z 0-9.  _ + - : () \" & * ? | ! {} [ ] ^ ~ \\ @ = # % $ ' /. Please try again.")
    return false;
  }
  
  return true;
}

bool checkAsterisk(QString query)
{
  int matches = query.indexOf(QRegExp("^[\*]*$|[\s]\*|^\*[^\s]"));
  if(matches != -1)
  {
    //if(alertUser) alert("Invalid search query! The wildcard (*) character must be preceded by at least one alphabet or number. Please try again.")
    return false;
  }
  return true;
}

bool checkAmpersands(QString query)
{
  // NB: doesn't handle term1 && term2 && term3 in Firebird 0.7
  int matches = query.indexOf(QRegExp("[&]{2}"));
  if(matches != -1)
  {
    matches = query.indexOf(QRegExp("^([a-zA-Z0-9_+\-:.()\"*?&|!{}\[\]\^~\\@#\/$%'=]+( && )?[a-zA-Z0-9_+\-:.()\"*?|!{}\[\]\^~\\@#\/$%'=]+[ ]*)+$")); // note missing & in pattern
    if(matches == -1)
    {
      //if(alertUser) alert("Invalid search query! Queries containing the special characters && must be in the form: term1 && term2. Please try again.")
      return false;
    }
  }
  return true;
}

bool checkCaret(QString query)
{
  //matches = query.indexOf(QRegExp("^[^\^]*$|^([a-zA-Z0-9_+\-:.()\"*?&|!{}\[\]\~\\@#\/]+(\^[\d]+)?[ ]*)+$")); // note missing ^ in pattern
  int matches = query.indexOf(QRegExp("[^\\]\^([^\s]*[^0-9.]+)|[^\\]\^$"));
  if(matches != null)
  {
    //if(alertUser) alert("Invalid search query! The caret (^) character must be preceded by alphanumeric characters and followed by numbers. Please try again.")
    return false;
  }
  return true;
}

bool checkSquiggle(QString query)
{
  //matches = query.indexOf(QRegExp("^[^~]*$|^([a-zA-Z0-9_+\-:.()\"*?&|!{}\[\]\^\\@#\/]+(~[\d.]+|[^\\]\\~)?[ ]*)+$")); // note missing ~ in pattern
  int matches = query.indexOf(QRegExp("[^\\]~[^\s]*[^0-9\s]+"));
  if(matches != null)
  {
    if(alertUser) alert("Invalid search query! The tilde (~) character must be preceded by alphanumeric characters and followed by numbers. Please try again.")
    return false;
  }    
  return true;
}

bool checkExclamationMark(QString query)
{
  // foo! is not a query, but !foo is. hmmmm...
  // NB: doesn't handle term1 ! term2 ! term3 or term1 !term2
  int matches = query.indexOf(QRegExp("^[^!]*$|^([a-zA-Z0-9_+\-:.()\"*?&|!{}\[\]\^~\\@#\/$%'=]+( ! )?[a-zA-Z0-9_+\-:.()\"*?&|!{}\[\]\^~\\@#\/$%'=]+[ ]*)+$"));
  if(matches == null || matches.length == 0)
  {
    if(alertUser) alert("Invalid search query! Queries containing the special character ! must be in the form: term1 ! term2. Please try again.")
    return false;
  }
  
  
  return true;
}

bool checkQuestionMark(QString query)
{
  int matches = query.indexOf(QRegExp("^(\?)|([^a-zA-Z0-9_+\-:.()\"*?&|!{}\[\]\^~\\@#\/$%'=]\?+)"));
  if(matches != null && matches.length > 0)
  {
      //if(alertUser) alert("Invalid search query! The question mark (?) character must be preceded by at least one alphabet or number. Please try again.")
    return false;
  }
  return true;
}

bool checkParentheses(QString query)
{
  bool hasLeft = false;
  bool hasRight = false;
  int matchLeft = query.indexOf(QRegExp("[(]"));
  if(matchLeft != -1) hasLeft = true
  int matchRight = query.indexOf(QRegExp("[)]"));
  if(matchRight != -1) hasRight = true;
  
  if(hasLeft || hasRight)
  {
    if(hasLeft && !hasRight || hasRight && !hasLeft)
    {
        //if(alertUser) alert("Invalid search query! Parentheses must be closed. Please try again.")
        return false;
    }
    else
    {
      //var number = matchLeft.length + matchRight.length;
      var number = matchLeft + matchRight;
      if((number % 2) > 0 || matchLeft != matchRight)
      {
        //if(alertUser) alert("Invalid search query! Parentheses must be closed. Please try again.")
        return false;
      }    
    }
    matches = query.indexOf(QRegExp("\(\)"));
    if(matches != -1)
    {
      //if(alertUser) alert("Invalid search query! Parentheses must contain at least one character. Please try again.")
      return false;    
    }
  }  
  return true;    
}

bool checkPlusMinus(QString query)
{
  matches = query.indexOf(QRegExp("^[^\n+\-]*$|^([+-]?[a-zA-Z0-9_:.()\"*?&|!{}\[\]\^~\\@#\/$%'=]+[ ]?)+$"));
  if(matches == -1)
  {
    //if(alertUser) alert("Invalid search query! '+' and '-' modifiers must be followed by at least one alphabet or number. Please try again.")
    return false;
  }
  return true;
}

bool checkANDORNOT(QString query)
{
  int matches = query.indexOf(QRegExp("AND|OR|NOT"));
  if(matches != -1)
  {
    matches = query.indexOf(QRegExp("^([a-zA-Z0-9_+\-:.()\"*?&|!{}\[\]\^~\\@\/#$%'=]+\s*((AND )|(OR )|(AND NOT )|(NOT ))?[a-zA-Z0-9_+\-:.()\"*?&|!{}\[\]\^~\\@\/#$%'=]+[ ]*)+$"));       
    if(matches == -1)
    {
      //if(alertUser) alert("Invalid search query!  Queries containing AND/OR/NOT must be in the form: term1 AND|OR|NOT|AND NOT term2 Please try again.")
      return false;
    }
    
    // its difficult to distinguish AND/OR/... from the usual [a-zA-Z] because they're...words!
    matches = query.indexOf(QRegExp("^((AND )|(OR )|(AND NOT )|(NOT ))|((AND)|(OR)|(AND NOT )|(NOT))[ ]*$"))
    if(matches != -1)
    {
      //if(alertUser) alert("Invalid search query!  Queries containing AND/OR/NOT must be in the form: term1 AND|OR|NOT|AND NOT term2 Please try again.")
      return false;
    }
  }
  return true;
}

bool checkQuotes(QString query)
{
  matches = query.indexOf(QRegExp("\""));
  if(matches != -1)
  {
    //var number = matches.length;
    if((matches % 2) > 0)
    {
      //if(alertUser) alert("Invalid search query! Please close all quote (\") marks.");
      return false;
    }
    matches = query.indexOf(QRegExp(""""));
    if(matches != -1)
    {
      //if(alertUser) alert("Invalid search query! Quotes must contain at least one character. Please try again.")
      return false;    
    }    
  }
  return true;
}

bool checkColon(QString query)
{
  int matches = query.indexOf(QRegExp("[^\\\s]:[\s]|[^\\\s]:$|[\s][^\\]?:|^[^\\\s]?:"));
  if(matches != -1)
  {
    //if(alertUser) alert("Invalid search query! Field declarations (:) must be preceded by at least one alphabet or number and followed by at least one alphabet or number. Please try again.")
    return false;
  }
  return true;
}