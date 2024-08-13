<?php
/**
 * The base configuration for WordPress
 *
 * The wp-config.php creation script uses this file during the installation.
 * You don't have to use the website, you can copy this file to "wp-config.php"
 * and fill in the values.
 *
 * This file contains the following configurations:
 *
 * * Database settings
 * * Secret keys
 * * Database table prefix
 * * ABSPATH
 *
 * @link https://developer.wordpress.org/advanced-administration/wordpress/wp-config/
 *
 * @package WordPress
 */

// ** Database settings - You can get this info from your web host ** //
/** The name of the database for WordPress */
define( 'DB_NAME', 'teplicaBD' );

/** Database username */
define( 'DB_USER', 'root' );

/** Database password */
define( 'DB_PASSWORD', '' );

/** Database hostname */
define( 'DB_HOST', 'localhost' );

/** Database charset to use in creating database tables. */
define( 'DB_CHARSET', 'utf8mb4' );

/** The database collate type. Don't change this if in doubt. */
define( 'DB_COLLATE', '' );

/**#@+
 * Authentication unique keys and salts.
 *
 * Change these to different unique phrases! You can generate these using
 * the {@link https://api.wordpress.org/secret-key/1.1/salt/ WordPress.org secret-key service}.
 *
 * You can change these at any point in time to invalidate all existing cookies.
 * This will force all users to have to log in again.
 *
 * @since 2.6.0
 */
define( 'AUTH_KEY',         'I7_ hSAN:<0s+[k#Ldo,UG9H1 U9:W-S%+r^1yMWB=c/@(dMwQ6,RN[-~pF}|R;t' );
define( 'SECURE_AUTH_KEY',  'ERl!QklF@WS1mz*N=St#7Bf*N3dNm &Z+<t4Yy.XpNAWIZC~f<2IV96Aq,Vqy=2-' );
define( 'LOGGED_IN_KEY',    'Z))Jf}s5@i=6ub0jBq-iFLg7o Bh%_)-4oN<s./?iWHw6o^N/.+bhU;42;;JOw3|' );
define( 'NONCE_KEY',        'LW!+*A#qU<`Y^8(]2(aXO7w,nQ+svVcM@?EdTOa=:[6b1G@/0+$U6_{CBRJ e(QU' );
define( 'AUTH_SALT',        'Yo9BKq*;[|Y,ZC&AY4C)92LnDoyh0/p4danKQ vMCV1it$>kzj}hzne)Nop,CEgj' );
define( 'SECURE_AUTH_SALT', 'e]88U@7[?N28jq<%cRBZjy]C!I7GZ~:R:zYs]xR73WhntzGw:+0:K^B-%9k?hJK9' );
define( 'LOGGED_IN_SALT',   'qDPtV*(zZ?dzZg;y{^sB_:#vwaAv@jJ@f+H]m}I$2L)tBM8,M]c>EeoSJ@3e&|zg' );
define( 'NONCE_SALT',       'Q`j|bG09KN_&hnj #zP=%I#mEJMlNh/hfA~>)y4sPdLHyX4h*~/*!x[ Uh+xJ8G>' );

/**#@-*/

/**
 * WordPress database table prefix.
 *
 * You can have multiple installations in one database if you give each
 * a unique prefix. Only numbers, letters, and underscores please!
 */
$table_prefix = 'wp_';

/**
 * For developers: WordPress debugging mode.
 *
 * Change this to true to enable the display of notices during development.
 * It is strongly recommended that plugin and theme developers use WP_DEBUG
 * in their development environments.
 *
 * For information on other constants that can be used for debugging,
 * visit the documentation.
 *
 * @link https://developer.wordpress.org/advanced-administration/debug/debug-wordpress/
 */
define( 'WP_DEBUG', false );

/* Add any custom values between this line and the "stop editing" line. */



/* That's all, stop editing! Happy publishing. */

/** Absolute path to the WordPress directory. */
if ( ! defined( 'ABSPATH' ) ) {
	define( 'ABSPATH', __DIR__ . '/' );
}

/** Sets up WordPress vars and included files. */
require_once ABSPATH . 'wp-settings.php';
