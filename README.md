SysThemer
=========

## Description ##
SysThemer is (will be) a system-wide theming utility for linux users who 
don't want to spend hours on changing every one of their program's configs
one by one. It uses its own universal theme format and does all the hard 
work for you!
    
## Dev priorities ##
| Feature                                                     | Priority | Status                               |
| ----------------------------------------------------------- | -------- | ------------------------------------ |
| Read from universal theme file                              | -------- | Done  |
| Read from a config file (to configure the systhemer itself) | -------- | Done   | 
| Search a config file for tokens                             | -------- | Done |
| Modify a config file with the snippets                      | High     | Started |
| Extract GTK theme into a universal theme                    | Low      |                                      |
| Support for the i3wm configs                                | Medium   | Started (for testing other features) |
| Support for other programs... (list to be continued)        | Medium         |                                      |
| Make the universal theme file a scripting/programming language of its own | Medium | Started (but still much less of a priority as other features) |
| Make a Cpp GUI frontend for this C commandline backend (kind of a universal control panel) | Low | |
    
## Usage ##
This section is more for myself and planning out how the code is gonna work
in the future but it serves as a good foundation for later actual
documentation for this program.
    
### Unitheme files ###
Unitheme files end with the extension .uth and it car store the following
things:
    
```
# comments;
begindef "program name"
variable = "value";
def list{
    "item1", "item2", 
    "item3"
}
enddef "program name";

```

#### Escape Characters ####
In the Uth format there are two types of escape characters:
- __outside strings:__ when trying to write a `#` or a `\` character outside of
  a string, you must add the escape character behind it: `\#` and `\\`.
- __insinde strings:__ when trying to write a `"` or a `\` character insinde of
  a string, you must add the escape character behind it: `\"` and `\\`.
    
#### Comments ####
Comments are written in the form `#comment`

#### Variables & Lists ####
Variables are defined like this: `VariableName = "Value"`
List variables are defined like this: `def ListName {items}`

##### Unitheme Variables #####
The available Uth variables are: 
- __tokens:__ defines what SysThemer should look for when scanning the file. Written
`def tokens {"^first\\s+\\S+$", "^\\S+\\s+second\\s+\\S+$"}` Here SysThemer would look for a line that
starts with `first` followed by two values and a line that contains a value
followed by the token `second` and then another value. There can be as many
values and tokens in any order to check for in one line as the user wants.
Systhemer now uses pcre2 for its file searching so please check out the Perl
compliant REGEX documentation at [their official website](http://perldoc.perl.org/perlre.html).
Some useful tools for testing out Regular
Expressions : [regex101.com](https://regex101.com/)
and [regexr.com](http://regexr.com/)
Also keep in mind the escape characters when copying your expressions to a list item in the uth
file. (see section on escape chars)

##### User Variables #####
_Coming soon... (check the Dev priorities section above for details. Look for
"Make the universal theme file a scripting/programming language of its own")_

#### Program Definitions ####
Begin a program's definition with the `begindef` statement, put your program's
properties (e.g.: define the path) then close it off with the `enddef` statement
as shown above.

### Incomplete Documentation ###
Systhemer is still in early development and so I have higher priorities at the
moment than writing documentation for a program that is just taking shape at the
moment. For example configurations and better understanding of the Unitheme file
format, navigate to the files folder and take a look at the
most recent version of the `default.uth` file or just download and give
Systhemer a try!
