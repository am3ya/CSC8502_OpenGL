#version 330 core

uniform sampler2D diffuseTex;
uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;

uniform vec4 light2Colour;
uniform vec3 light2Pos;
uniform float light2Radius;

in Vertex {
   vec3 colour;
   vec2 texCoord;
   vec3 normal;
   vec3 worldPos;
} IN;

out vec4 fragColour;

void main(void){
   vec3 incident = normalize(lightPos - IN.worldPos);
   vec3 viewDir = normalize(cameraPos - IN.worldPos);
   vec3 halfDir = normalize(incident + viewDir);

   vec4 diffuse = texture(diffuseTex, IN.texCoord);

   float lambert = max(dot(incident, IN.normal), 0.0f);
   float Distance = length(lightPos - IN.worldPos);
   float attenuation = 1.0 - clamp(Distance / lightRadius, 0.0, 1.0);

   float specFactor = clamp(dot(halfDir, IN.normal), 0.0, 1.0);
   specFactor = pow(specFactor, 60.0);

   vec3 surface = (diffuse.rgb * lightColour.rgb);
   vec3 light1Result/*fragColour.rgb*/ = surface * lambert * attenuation;
   light1Result/*fragColour.rgb*/ += (lightColour.rgb * specFactor)*attenuation*0.33;
   //fragColour.rgb += surface * 0.1f; //Ambient lighting
   //fragColour.a = diffuse.a;

   //fragColour = vec4(1.0,0.0,0.0,1.0);

   vec3 incident2 = normalize(light2Pos - IN.worldPos);  // Light 2 direction
   vec3 viewDir2 = normalize(cameraPos - IN.worldPos);   // View direction
   vec3 halfDir2 = normalize(incident2 + viewDir2);      // Halfway vector

   float lambert2 = max(dot(incident2, IN.normal), 0.0);   // Diffuse term
   float distance2 = length(light2Pos - IN.worldPos);      // Distance to light 2
   float attenuation2 = 1.0 - clamp(distance2 / light2Radius, 0.0, 1.0); // Attenuation

   float specFactor2 = pow(max(dot(halfDir2, IN.normal), 0.0), 60.0);   // Specular term

   vec3 light2Surface = (diffuse.rgb * light2Colour.rgb); // Light 2 contribution
   vec3 light2Result = (light2Surface * lambert2 * attenuation2) + 
                        (light2Colour.rgb * specFactor2 * attenuation2 * 0.33);

   // Combine light contributions and add ambient lighting
   vec3 ambient = diffuse.rgb * 0.1; // Ambient term
   fragColour.rgb = light1Result + light2Result + ambient;
   fragColour.a = diffuse.a; // Preserve alpha from texture
}