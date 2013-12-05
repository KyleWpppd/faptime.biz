<?php

interface FaptimeStatus
{
    const ACTIVE = 0;
    const DELETED_UNKNOWN = 1;
    const SPAM = 2;
    const DMCA = 3;
    const TOS_VIOLATION = 4;
    const TEMPORARY_REMOVAL = 5;
}

class FaptimeUrl
{
    protected $id = null;
    protected $status = FaptimeStatus::ACTIVE;
    protected $url = '';
    protected $urlHash = '';
}

class FaptimeUrlHash
{
    protected $id = null;
    protected $status = FaptimeStatus::ACTIVE;
    protected $hash = '';
    protected $urlId = '';
    protected $userId = null;
}

class FaptimeServer {
    function saveUrl($url) {
        $id = $this->insertUrl($url);
        if (! $id) { return false; }

        try {
            $hash = $this->getHash($id);
        } catch (Exception $e) { return false; }

        if (! $hash) {
            $hash = $this->generateHash($id);
        }

        if (! $hash) { return false; }

        return $hash;
    }

    function insertUrl($url) {
        // This is not atomic and it needs to be...
        $urlHash = $this->hashUrl($url);
        $exists = $this->urlExists($url, $urlHash); // ideally this would hash
        if ($exists && $exists['url'] === $url) {
            return $exists['id'];
        }

        $url = new FaptimeUrl($url, $currentUser);
        $id = $this->db()->insert($url);
        // then do process for hash
        return $id;
    }

    function insertHash($urlId) {
        // hash is a unique key
        $fh = new FaptimeUrlHash;
        $hashId = null;
        $limit = 3;
        do {
            $hash = $fh->newRandomHash();
            $hashId = $this->db()->insert_hash($hash, $urlId);
            $attempts++;
        } while (! $hashId && $attempts < $limit);
        return $hashId;
    }
}