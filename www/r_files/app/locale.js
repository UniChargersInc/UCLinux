var lleStrings={    
	sLogout:{ 
	'en':'Logout',
	'es':'Salir',
	'fr':'Sortir',
	
	},   
	sLogin:{ 
	'en':'Login',
	'es':'Entrar',
	'fr':'Entrer',
	},
    sRem:{ 
	'en':'Remember me',
	'es':'Recuérdame',
	'fr':'Se souvenir de moi',
	},
	
	sUser:{ 
	'en':'Username',
	'es':'Usuario',
	'fr':'Utilisateur',
	
	},
	sPass:{ 
	'en':'Password',
	'es':'Contraseña',
	'fr':'Mot de passe',
	
	},
	sLang:{ 
	'en':'Language',
	'es':'Idioma',
	'fr':'Langage',
	
	},
	sBack:{
		'en':'Back',
		'es':'Atrás',
		'fr':'Arrière',
		
	},
	sCmd:{
		'en':'Command',
		'es':'Comando',
		'fr':'Commandement',
		
	},
	sBCT:{
		'en':'Breadcrumb Trail',
		'es':'Breadcrumb camino',
		'fr':'Breadcrumb sentier',
		
	},
	sLoading:{
		'en':'Loading..',
		'es':'Cargando..',
		'fr':'Chargement..',
		
	},
	sAss:{
		'en':'Assets',
		'es':'Bienes',
		'fr':'Actif',
		
	},
	sUserOpt:{
		'en':'User options',
		'es':'Opciones',
		'fr':'Les options',
		
	},
	sNav:{
		'en':'Navigation',
		'es':'Navegación',
		'fr':'Navigation',
		
	},
	sClose:{
		'en':'Close',
		'es':'Cerrar',
		'fr':'Fermer',
		
	},
	sAccept:{
		'en':'Command Accepted',
		'es':'Comando aceptado',
		'fr':'Accepter la commande',
		
	},
	sSTr:{
		'en':'Show trail',
		'es':'Mostrar trail',
		'fr':'Afficher trail',
		
	},
	sClr:{
		'en':'Clear',
		'es':'Borrar',
		'fr':'Effacer',
		
	},
	
	sDoor:{
		'en':'"Unlock the Door"',
		'es':'Seguros de Puerta',
		'fr':'Déverrouiller la porte',
		
	},
	sEEng:{
		'en':'"Enable starter"',
		'es':'Habilitar el arranque',
		'fr':'Activer démarrage',
		
	},
	sDEng:{
		'en':'"Disable starter"',
		'es':'Desactivar arranque',
		'fr':'Désactiver démarrage',
		
	},
	sSCE:{
		'en':'Send Command Error:',
		'es':'Error de envío de comandos:',
		'fr':'Envoyer Erreur de commande:'
	},
	sClust:{
		'en':'This cluster has',
		'es':'Este grupo tiene',
		'fr':'Ce pôle a'
	},
	sItms:{
		'en':'items',
		'es':'artículos',
		'fr':'articles'
	},
	sRsr:{
		'en':'Mobile Portal',
		'es':'Portal Móvil',
		'fr':'Portail Mobile'
	},
	
};

var lle='en'; 
function setLang(i){if(i==1)lle="en";else if(i==3)lle="fr";else if(i==2)lle="es";}

function getLStr(s)
{
	var v="";
	try{
		v=lleStrings[s][lle];
	}
	catch(e)
	{
		v="error";
	}
	return v;
}
function getlang()
{
	var i=1;
	if(lle=="fr")i=3;else if(lle=="es")i=2;
	return i;
}
