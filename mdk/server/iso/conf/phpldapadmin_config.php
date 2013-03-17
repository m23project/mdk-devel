<?php


/*
 *                  The phpLDAPadmin config file
 *
 *   This is where you customize phpLDAPadmin. The most important
 *   part is immediately below: The "LDAP Servers" section.
 *   You must specify at least one LDAP server there. You may add
 *   as many as you like. You can also specify your language, and
 *   many other options.
 *
 */

/**
 * phpLDAPadmin can encrypt the content of sensitive cookies if you set this
 * to a big random string.
 */
$blowfish_secret = '';

// Your LDAP servers
$i=0;
$servers = array();
//['unique_attrs_dn_pass']

// If you want to configure more LDAP servers, copy and paste the above (including the "$i++;")

// The temporary storage directory where we will put jpegPhoto data
// This directory must be readable and writable by your web server
$jpeg_temp_dir = "/tmp";       // Example for Unix systems


/**                            **/
/**   Appearance and Behavior  **/
/**                            **/

// Whenever we display a date use this format.
$date_format = "%A %e %B %Y";

// Set this to true if you want to hide the Request New Feature and Report bugs.
$hide_configuration_management = false;


// A format string used to display enties in the tree viewer (left-hand side)
// You can use special tokens to draw the entries as you wish. You can even mix in HTML to format the string
// Here are all the tokens you can use:
//      %rdn - draw the RDN of the entry (ie, "cn=Dave")
//      %dn - draw the DN of the entry (ie, "cn=Dave,ou=People,dc=example,dc=com"
//      %rdnValue - draw the value of the RDN (ie, instead of "cn=Dave", just draw "Dave")
//      %[attrname]- draw the value (or values) of the specified attribute.
//              examle: %gidNumber
$tree_display_format = '%rdn';
//
// Examples:
//
// To draw the gidNumber and uidNumber to the right of the RDN in a small, gray font:
//$tree_display_format = '%rdn <small style="color:gray">( %gidNumber / %uidNumber )</span>';
// To draw the full DN of each entry:
//$tree_display_format = '%dn';
// To draw the objectClasses to the right in parenthesis:
//$tree_display_format = '%rdn <small style="color: gray">( %objectClass )</small>';
// To draw the user-friendly RDN value (ie, instead of "cn=Dave", just draw "Dave"):
//$tree_display_format = '%rdnValue';


// Aliases and Referrrals
//
// Similar to ldapsearh's -a option, the following options allow you to configure
// how phpLDAPadmin will treat aliases and referrals in the LDAP tree.
// For the following four settings, avaialable options include:
//
//    LDAP_DEREF_NEVER     - aliases are never dereferenced (eg, the contents of
//                           the alias itself are shown and not the referenced entry).
//    LDAP_DEREF_SEARCHING - aliases should be dereferenced during the search but
//                           not when locating the base object of the search.
//    LDAP_DEREF_FINDING   - aliases should be dereferenced when locating the base
//                           object but not during the search.
//    LDAP_DEREF_ALWAYS    - aliases should be dereferenced always (eg, the contents
//                           of the referenced entry is shown and not the aliasing entry)

// How to handle references and aliases in the search form. See above for options.
$search_deref = LDAP_DEREF_ALWAYS;

// How to handle references and aliases in the tree viewer. See above for options.
$tree_deref = LDAP_DEREF_NEVER;

// How to handle references and aliases for exports. See above for options.
$export_deref = LDAP_DEREF_NEVER;

// How to handle references and aliases when viewing entries. See above for options.
$view_deref = LDAP_DEREF_NEVER;


// The language setting. If you set this to 'auto', phpLDAPadmin will
// attempt to determine your language automatically. Otherwise, available
// lanaguages are: 'ct', 'de', 'en', 'es', 'fr', 'it', 'nl', and 'ru'
// Localization is not complete yet, but most strings have been translated.
// Please help by writing language files. See lang/en.php for an example.
$language = 'auto';

// Set to true if you want to draw a checkbox next to each entry in the tree viewer
// to be able to delete multiple entries at once
$enable_mass_delete = false;

// Set to true if you want LDAP data to be displayed read-only (without input fields)
// when a user logs in to a server anonymously
$anonymous_bind_implies_read_only = true;

// Set to true if you want phpLDAPadmin to redirect anonymous
// users to a search form with no tree viewer on the left after
// logging in.
$anonymous_bind_redirect_no_tree = false;

// If you used auth_type 'form' in the servers list, you can adjust how long the cookie will last
// (default is 0 seconds, which expires when you close the browser)
$cookie_time = 0; // seconds

// How many pixels wide do you want your left frame view (for the tree browser)
$tree_width = 320; // pixels

// How long to keep jpegPhoto temporary files in the jpeg_temp_dir directory (in seconds)
$jpeg_tmp_keep_time = 120; // seconds

// Would you like to see helpful hint text occacsionally?
$show_hints = true; // set to false to disable hints

// When using the search page, limit result size to this many entries
$search_result_size_limit = 50;

// By default, when searching you may display a list or a table of results.
// Set this to 'table' to see table formatted results.
// Set this to 'list' to see "Google" style formatted search results.
$default_search_display = 'list';

// If true, display all password hash values as "******". Note that clear-text
// passwords will always be displayed as "******", regardless of this setting.
$obfuscate_password_display = false;

/**                              **/
/** Simple Search Form Config **/
/**                              **/

// Which attributes to include in the drop-down menu of the simple search form (comma-separated)
// Change this to suit your needs for convenient searching. Be sure to change the corresponding
// list below ($search_attributes_display)
$search_attributes = "uid, cn, gidNumber, objectClass, telephoneNumber, mail, street";

// This list corresponds to the list directly above. If you want to present more readable names
// for your search attributes, do so here. Both lists must have the same number of entries.
$search_attributes_display = "User Name, Common Name, Group ID, Object Class, Phone Number, Email, Address";

// The list of attributes to display in each search result entry.
// Note that you can add * to the list to display all attributes
$search_result_attributes = "cn, sn, uid, postalAddress, telephoneNumber";

// You can re-arrange the order of the search criteria on the simple search form by modifying this array
// You cannot however change the names of the criteria. Criteria names will be translated at run-time.
$search_criteria_options = array( "equals", "starts with", "contains", "ends with", "sounds like" );

// If you want certain attributes to be editable as multi-line, include them in this list
// A multi-line textarea will be drawn instead of a single-line text field
$multi_line_attributes = array( "postalAddress", "homePostalAddress", "personalSignature" );

// A list of syntax OIDs which support multi-line attribute values:
$multi_line_syntax_oids = array(
                            // octet string syntax OID:
                            "1.3.6.1.4.1.1466.115.121.1.40",
                            // postal address syntax OID:
                            "1.3.6.1.4.1.1466.115.121.1.41"  );

/**                                         **/
/** User-friendly attribute translation     **/
/**                                         **/

$friendly_attrs = array();

// Use this array to map attribute names to user friendly names. For example, if you
// don't want to see "facsimileTelephoneNumber" but rather "Fax".

$friendly_attrs[ 'facsimileTelephoneNumber' ] =         'Fax';
$friendly_attrs[ 'telephoneNumber' ]  =                 'Phone';

/**                                         **/
/** support for attrs display order         **/
/**                                         **/

// Use this array if you want to have your attributes displayed in a specific order.
// You can use default attribute names or their fridenly names.
// For example, "sn" will be displayed right after "givenName". All the other attributes
// that are not specified in this array will be displayed after in alphabetical order.

// $attrs_display_order = array(
// 				"givenName",
// 				"sn",
// 				"cn",
// 				"displayName",
// 				"uid",
// 				"uidNumber",
// 				"gidNumber",
// 				"homeDirectory",
// 				"mail",
// 				"userPassword"
// );

/**                                         **/
/** Hidden attributes                       **/
/**                                         **/

// You may want to hide certain attributes from being displayed in the editor screen
// Do this by adding the desired attributes to this list (and uncomment it). This
// only affects the editor screen. Attributes will still be visible in the schema
// browser and elsewhere. An example is provided below:
// NOTE: The user must be able to read the hidden_except_dn entry to be excluded.

//$hidden_attrs = array( 'jpegPhoto', 'objectClass' );
//$hidden_except_dn = "cn=PLA UnHide,ou=Groups,c=AU";

// Hidden attributes in read-only mode. If undefined, it will be equal to $hidden_attrs.
//$hidden_attrs_ro = array( 'objectClass','shadowWarning', 'shadowLastChange', 'shadowMax',
//                          'shadowFlag', 'shadowInactive', 'shadowMin', 'shadowExpire' );

/**                                         **/
/** Read-only attributes                    **/
/**                                         **/

// You may want to phpLDAPadmin to display certain attributes as read only, meaning
// that users will not be presented a form for modifying those attributes, and they
// will not be allowed to be modified on the "back-end" either. You may configure
// this list here:
// NOTE: The user must be able to read the read_only_except_dn entry to be excluded.

//$read_only_attrs = array( 'objectClass' );
//$read_only_except_dn = "cn=PLA ReadWrite,ou=Groups,c=AU";

// An example of how to specify multiple read-only attributes:
// $read_only_attrs = array( 'jpegPhoto', 'objectClass', 'someAttribute' );

/**                                         **/
/** Unique attributes                       **/
/**                                         **/
// You may want phpLDAPadmin to enforce some attributes to have unique values (ie:
// not belong to other entries in your tree. This (together with "unique_attrs_dn"
// and "unique_attrs_dn_pass" option will not let updates to occur with other attributes
// have the same value.
// NOTE: Currently the unique_attrs is NOT enforced when copying a dn. (Need to present a user with
// the option of changing the unique attributes.
//$unique_attrs = array('uid','uidNumber','mail');

/**                                         **/
/** Predefined Queries (canned views)       **/
/**                                         **/

// To make searching easier, you may setup predefined queries below (activate the lines by removing "//")
$q=0;
$queries = array();
$queries[$q]['name'] = 'Samba Users';       /* The name that will appear in the simple search form */
$queries[$q]['server'] = '0';               /* The ldap server to query, must be defined in the $servers list above */
$queries[$q]['base'] = 'dc=example,dc=com'; /* The base to search on */
$queries[$q]['scope'] = 'sub';              /* The search scope (sub, base, one) */
$queries[$q]['filter'] = '(&(|(objectClass=sambaAccount)(objectClass=sambaSamAccount))(objectClass=posixAccount)(!(uid=*$)))';
                                          /* The LDAP filter to use */
$queries[$q]['attributes'] = 'uid, smbHome, uidNumber';
                                            /* The attributes to return */

// Add more pre-defined queries by copying the text below
$q++;
$queries[$q]['name'] = 'Samba Computers';
$queries[$q]['server'] = '0';
$queries[$q]['base'] = 'dc=example,dc=com';
$queries[$q]['scope'] = 'sub';
$queries[$q]['filter'] = '(&(objectClass=sambaAccount)(uid=*$))';
$queries[$q]['attributes'] = 'uid, homeDirectory';


?>
