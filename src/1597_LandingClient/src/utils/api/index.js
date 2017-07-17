//TODO XXX change to correct IP here
// const baseUrl = 'http://192.168.178.43:3000';
// const baseUrl = 'http://localhost:3000';
// const baseUrl = 'http://10.52.248.92:3000';
// const baseUrl = 'http://localhost:3000';
//TODO: Das klappt hier noch gar nicht. Das muss irgendwie im now-build eingetragen werden...
const baseUrl=( process.env.NOW || process.env.npm_config_NOW || process.argv.indexOf('--NOW') != -1)?
    'https://umlageserver.now.sh':
    'http://localhost:3000';

const apiPrefix = 'api';

class AuthenticationRequiredError extends Error {
    constructor(message) {
        super(message);
        this.message = message;
        this.name = 'AuthenticationRequiredError';
        this.type = 'ApiError';
    }
}

class AuthorizationRequiredError extends Error {
    constructor(message) {
        super(message);
        this.message = message;
        this.name = 'AuthorizationRequiredError';
        this.type = 'ApiError';
    }
}

class InvalidArgumentsError extends Error {
    constructor(message) {
        super(message);
        this.message = message;
        this.name = 'InvalidArgumentsError';
        this.type = 'ApiError';
    }
}

class UnknownApiError extends Error {
    constructor(message) {
        super(message);
        this.message = message;
        this.name = 'UnknownApiError';
        this.type = 'ApiError';
    }
}

function createGetRequest() {
    const headers = new Headers({
        'Content-Type': 'application/json',
    });
;
    return {
        headers,
        method: 'GET',
        credentials: 'include',
    };
}

function createPostRequest(payload) {
    const headers = new Headers({
        'Content-Type': 'application/json',
    });

    return {
        headers,
        method: 'POST',
        body: JSON.stringify(payload),
        credentials: 'include',
    };
}

function createPatchRequest(payload) {
    const headers = new Headers({
        'Content-Type': 'application/json',
    });

    return {
        headers,
        method: 'PATCH',
        body: JSON.stringify(payload),
        credentials: 'include',
    };
}

export const getFileInfo = (filename) => {
    const payload = {
        filename,
    };

    return fetch(`${baseUrl}/${apiPrefix}/fileinfo`, createPostRequest(payload))
        .then((response) => {
            if (response.status === 200) return response.json();

            if (response.status === 400) throw new InvalidArgumentsError('Could not get file info for "'+filename+'"');

            throw new Error('Other server error');
        });
};

export const getFileExtent = (filename) => {
    const payload = {
        filename,
    };

    return fetch(`${baseUrl}/${apiPrefix}/fileinfo/extent`, createPostRequest(payload))
        .then((response) => {
            if (response.status === 200) return response.json();

            if (response.status === 400) throw new InvalidArgumentsError('Could not get file extents for "'+filename+'"');

            throw new Error('Other server error');
        });
};

export const sendTask2Server = (scanTask) =>{
    const payload = scanTask;

    return fetch(`${baseUrl}/${apiPrefix}/commands`, createPostRequest(payload))
        .then((response) => {
            if (response.status === 200) return response.scanTask;

            if (response.status === 400) throw new InvalidArgumentsError('Could send task to server for task ', scanTask);

            throw new Error('Other server error');
        });
};
