#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;
in vec4 fPosEye;
in vec4 fragPosLightSpace;

out vec4 fColor;

uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;

uniform vec3 dirLumina;
uniform vec3 culoareLumina;
uniform vec3 culoareLuminaFelinare;
uniform vec3 coordonateFelinare[4];

uniform float constant;
uniform float linear;
uniform float quadratic;

uniform int ziNoapte;

uniform sampler2D texturaDifuza;
uniform sampler2D texturaSpeculara;
uniform sampler2D shadowMap;

uniform float intensitateCeata;

vec3 ambient,ambientFelinare;
vec3 diffuse,diffuseFelinare;
vec3 specular,specularFelinare;

float putereAmbient = 0.4f;
float putereSpecular = 0.5f;
float stralucireaSpeculara= 32.0f;

float putereAmbientFelinare = 20.0f;
float putereSpecularFelinare = 10.0f;

void calculCuloare()
{
    vec3 normSpCamera = normalize(normalMatrix * fNormal);	
    vec3 dirLuminaN = vec3(normalize(view * vec4(dirLumina, 0.0f)));	
    vec3 dirVizualizare = normalize(vec3(0.0f) - fPosEye.xyz);

	//componenta ambientala
    ambient = putereAmbient * culoareLumina;

	//componenta difuza
    diffuse = max(dot(normSpCamera, dirLuminaN), 0.0f) * culoareLumina;
  
    vec3 dirReflectata = normalize(reflect(-dirLuminaN, normSpCamera));
    float coefSpecular = pow(max(dot(dirVizualizare, dirReflectata), 0.0f), stralucireaSpeculara);
    specular = putereSpecular * coefSpecular * culoareLumina;
}

void calculCuloareCuFelinare()
{
	ambientFelinare = vec3(0.0f);
	diffuseFelinare = vec3(0.0f);
	specularFelinare = vec3(0.0f); 
	
	for(int i=0; i<=3; i++){

		vec3 normalEye = normalize(fNormal);	

		vec4 lightPosEye1 = view * model * vec4(coordonateFelinare[i], 1.0f);
		vec3 lightPosEye = lightPosEye1.xyz;

		vec3 dirLuminaN = normalize(fPosEye.xyz - lightPosEye);	
		//directia de vizualizare normalizata
		vec3 viewDirN = normalize(-fPosEye.xyz);
		//reflexia luminii
		vec3 reflection = normalize(reflect(dirLuminaN, normalEye));
		
		//coef speculara
		float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), stralucireaSpeculara);
		
		float distantaL = length(lightPosEye - fPosEye.xyz); 
		//atenuarea in functie de distanta pe baza formulei
		float atenuareDist = 1.0 / (constant + linear * distantaL + quadratic * (distantaL * distantaL));
					 
		vec3 ambientp = atenuareDist * putereAmbientFelinare * culoareLuminaFelinare;
		vec3 diffusep = atenuareDist * max(dot(normalEye, dirLuminaN), 0.0f) * culoareLuminaFelinare;
		vec3 specularp = atenuareDist * putereSpecularFelinare * specCoeff * culoareLuminaFelinare;
		
		ambientp *= texture(texturaDifuza, fTexCoords);
		diffusep *= texture(texturaDifuza, fTexCoords);
		specularp *= texture(texturaSpeculara, fTexCoords);
	
		ambientFelinare += ambientp;
		diffuseFelinare += diffusep;
		specularFelinare += specularp;
	}
}

float calculCoeficientUmbre()
{
	float umbraCoeficient = 0.0f;
	//transformare in coordonate lumina
	vec3 coordNormalizate = fragPosLightSpace.xyz / fragPosLightSpace.w;
	coordNormalizate = coordNormalizate * 0.5 + 0.5;
	//verificare daca este in afara frustului de lumina
	if (coordNormalizate.z > 1.0f)
		return 0.0f;
	//calcul adancime apropiere
	float adanAprop = texture(shadowMap, coordNormalizate.xy).r;
	//calcul adancime curenta
	float adanCur = coordNormalizate.z;
	//tratarea cazului de shadow acne
	float bias = max(0.05f * (1.0f - dot(fNormal, dirLumina)), 0.005f);
	if (adanCur - bias > adanAprop)
		umbraCoeficient = 1.0f;
		
	return umbraCoeficient;
}

float calculCoeficientCeata()
{
   //calculare distanta dintre poz camera si fragment
   float distantaFrag = length(fPosEye);
   //calcul coeficient ceata
   float ceataCoef = exp(-pow(distantaFrag * intensitateCeata, 2));
   return clamp(ceataCoef, 0.0f, 1.0f);
}

void main() 
{
	float umbraCoeficient;
    ambient = vec3(0.0f);
	diffuse = vec3(0.0f);
	specular = vec3(0.0f); 
	 
	if( ziNoapte == 1 ) {
		//in peisajul de zi
		calculCuloare();
		ambient *= texture(texturaDifuza, fTexCoords).rgb;
		diffuse *= texture(texturaDifuza, fTexCoords).rgb;
		specular *= texture(texturaSpeculara, fTexCoords).rgb;
	}
	else{	
		//calcul culoare fragment cu felinare
		calculCuloareCuFelinare();		
		ambient += ambientFelinare;
		diffuse += diffuseFelinare;
		specular += specularFelinare;
	}	

		
	vec3 culoare = min((ambient + diffuse) + specular , 1.0f);
	
	if( ziNoapte == 1 ){
		//calcul coeficient umbre
		umbraCoeficient = calculCoeficientUmbre();
		//interpolare culoare
		culoare = min((ambient + (1.0f - umbraCoeficient) * diffuse) + (1.0f - umbraCoeficient) * specular, 1.0f);
	}	

	//stabilire coeficient ceata
	float ceataCoeficient = calculCoeficientCeata();	
	//interpolare culoare finala fragment
	fColor = mix(vec4(0.5f, 0.5f, 0.5f, 1.0f), vec4(culoare, 1.0f), ceataCoeficient);
}