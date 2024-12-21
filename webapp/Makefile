

# configure docker user
DOCKER_USER = ${shell id -u}:${shell id -g}


.PHONY: ng-serve
ng-serve:
	@DOCKER_USER=$(DOCKER_USER) \
	docker compose run --rm angular \
		ng serve

ng-build:
	@DOCKER_USER=$(DOCKER_USER) \
	docker compose run --rm angular \
		ng build --base-href = "/app/"
