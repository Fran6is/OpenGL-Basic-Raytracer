
HitResult PlaneRayIntersection(FSphere SceneSphere, vec3 RayPosition, vec3 RayDirection)
{

}

HitResult SphereRayIntersection(FSphere SceneSphere, vec3 RayPosition, vec3 RayDirection)
{
	float DistanceToSphere = dot(SceneSphere.Location - RayPosition, RayDirection);

	HitResult HitResult;
	HitResult.bWasHit = false;
    const float DISTANCE = 1.0;

	if(DistanceToSphere >= DISTANCE ) //Must be at least the specified distance "in front" of the camera. Not behind or too close
	{
		float Y = length( (RayPosition + RayDirection * DistanceToSphere) - SceneSphere.Location );

		if(Y <= SceneSphere.Radius)
		{
			float X = sqrt(SceneSphere.Radius * SceneSphere.Radius - Y * Y);

            float T1 = DistanceToSphere - X; //first point of ray intersection with the sphere
            float T2 = DistanceToSphere + X; //second point of intersection/ray exit point

			HitResult.bWasHit = true;
			HitResult.HitLocation = RayPosition + RayDirection * T1;
			HitResult.HitNormal = normalize( HitResult.HitLocation - SceneSphere.Location );
			HitResult.Distance  = T1;

			return HitResult;
		}
		return HitResult;
	}

	return HitResult;
}